// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/ngram-counter.h>

#include <nwc-toolkit/multikey-sort.h>
#include <nwc-toolkit/token-trie-tracer.h>

#include <limits>

namespace nwc_toolkit {
namespace {

class PairKeyHandler {
 public:
  unsigned char operator()(const std::pair<nwc_toolkit::String, int> &pair,
      std::size_t index) const {
    return static_cast<unsigned char>(pair.first[index]);
  }
  bool operator()(unsigned char c) const {
    return c == '\0';
  }
};

class IntKeyHandler {
 public:
  int operator()(const int *key, std::size_t index) const {
    return (key[index] >= 0) ? key[index] : -1;
  }
  bool operator()(int c) const {
    return c == -1;
  }
};

}  // namespace

NgramCounter::NgramCounter()
    : input_format_(DEFAULT_FORMAT),
      with_boundary_count_(false),
      with_result_sort_(false),
      memory_limit_(0),
      token_map_(),
      token_trie_(),
      tokens_(),
      token_count_(0),
      sentence_count_(0) {
  Reset(0, DEFAULT_MEMORY_LIMIT);
}

void NgramCounter::Reset(std::size_t max_ngram_length,
    std::size_t memory_limit) {
  if (max_ngram_length == 0) {
    max_ngram_length = TokenTrie::DEFAULT_MAX_DEPTH;
  } else if (max_ngram_length < TokenTrie::MIN_MAX_DEPTH) {
    max_ngram_length = TokenTrie::MIN_MAX_DEPTH;
  }

  if (memory_limit == 0) {
    memory_limit = DEFAULT_MEMORY_LIMIT;
  } else if (memory_limit < MIN_MEMORY_LIMIT) {
    memory_limit = MIN_MEMORY_LIMIT;
  }

  std::size_t token_trie_memory_usage = memory_limit;
  if (with_result_sort()) {
    token_trie_memory_usage =
        memory_limit / (164 + (13 * max_ngram_length)) * 100;
  }
  token_trie_.Reset(max_ngram_length, token_trie_memory_usage);
  memory_limit_ = memory_limit;

  token_count_ = 0;
  sentence_count_ = 0;

  Clear();
}

void NgramCounter::Clear() {
  token_map_.Clear();
  if (with_result_sort()) {
    token_map_.Insert("<S>");
    token_map_.Insert("</S>");
  }
  token_trie_.Clear();
}

bool NgramCounter::Count(InputFile *input_file) {
  if (!ReadTokens(input_file)) {
    return false;
  }
  token_trie_.Insert(&tokens_[0], tokens_.size());
  token_count_ += tokens_.size();
  ++sentence_count_;
  return true;
}

bool NgramCounter::NeedsFlush() {
  // In order to avoid overflow, ngrams should be flushed when the maximum
  // frequency becomes very large. Normally, a frequency does not reach
  // the half of int's maximum value.
  if (token_trie_.max_freq() >= (std::numeric_limits<int>::max() / 2)) {
    return true;
  }

  // In order to keep the performance, ngrams should be flushed if there
  // are not enough spaces in the hash table of TokenTrie.
  if (!token_trie_.is_empty() &&
      ((1.0 * token_trie_.num_nodes() / token_trie_.table_size()) >= 0.8)) {
    return true;
  }

  // Sorting of result n-grams need a large memory space, thus some parameters
  // of TokenTrie should be checked.
  if (with_result_sort()) {
    std::size_t list_mem_usage = token_trie_.total_length() * sizeof(int)
        + token_trie_.num_nodes() * sizeof(int);
    if ((list_mem_usage + token_trie_.memory_usage()) > memory_limit()) {
      return true;
    }
  }
  return false;
}

bool NgramCounter::Flush(OutputFile *output_file) {
  bool ret = false;
  if (with_result_sort()) {
    ret = FlushWithSort(output_file);
  } else {
    ret = FlushWithoutSort(output_file);
  }
  Clear();
  return ret;
}

bool NgramCounter::ReadTokens(InputFile *input_file) {
  tokens_.clear();
  if (with_boundary_count()) {
    tokens_.push_back(START_TOKEN_ID);
  }
  switch (input_format()) {
    case WAKATI_FORMAT: {
      if (!ReadWakatiTokens(input_file)) {
        return false;
      }
      break;
    }
    case MECAB_FORMAT: {
      if (!ReadMecabTokens(input_file)) {
        return false;
      }
      break;
    }
    case CHASEN_FORMAT: {
      if (!ReadChasenTokens(input_file)) {
        return false;
      }
      break;
    }
    default: {
      return false;
    }
  }
  if (with_boundary_count()) {
    tokens_.push_back(END_TOKEN_ID);
  }
  return true;
}

bool NgramCounter::ReadWakatiTokens(InputFile *input_file) {
  std::size_t num_tokens = tokens_.size();
  nwc_toolkit::String line;
  while (input_file->ReadLine(&line)) {
    line = line.StripRight();
    while (!line.is_empty()) {
      nwc_toolkit::String delim = line.FindFirstOf(' ');
      nwc_toolkit::String token(line.begin(), delim.begin());
      if (!token.is_empty()) {
        tokens_.push_back(token_map_.Insert(token));
      }
      line.set_begin(delim.end());
    }
    // Skips empty sentences.
    if (tokens_.size() > num_tokens) {
      return true;
    }
  }
  return false;
}

bool NgramCounter::ReadMecabTokens(InputFile *input_file) {
  std::size_t num_tokens = tokens_.size();
  nwc_toolkit::String line;
  while (input_file->ReadLine(&line)) {
    line = line.StripRight();
    if (line == "EOS") {
      // Skips empty sentences.
      if (tokens_.size() > num_tokens) {
        return true;
      } else {
        continue;
      }
    }
    nwc_toolkit::String delim = line.FindFirstOf('\t');
    nwc_toolkit::String token(line.begin(), delim.begin());
    if (!token.is_empty()) {
      tokens_.push_back(token_map_.Insert(token));
    }
  }
  return false;
}

bool NgramCounter::ReadChasenTokens(InputFile *input_file) {
  return ReadMecabTokens(input_file);
}

bool NgramCounter::FlushWithSort(OutputFile *output_file) {
  std::vector<int> token_freqs;
  token_freqs.reserve(token_trie_.total_length() + token_trie_.num_nodes());
  token_freqs.push_back(-1);

  nwc_toolkit::TokenTrieTracer token_trie_tracer;
  token_trie_tracer.Trace(token_trie_);
  int freq;
  while (token_trie_tracer.Next(&token_freqs, &freq)) {
    token_freqs.push_back(-freq);
  }
  token_trie_tracer.Clear();

  std::size_t num_keys = token_trie_.num_nodes() - 1;
  token_trie_.Clear();

  std::vector<std::pair<nwc_toolkit::String, int> > pairs;
  pairs.resize(token_map_.num_tokens());
  for (std::size_t i = 0; i < pairs.size(); ++i) {
    pairs[i].first = token_map_[i];
    pairs[i].second = static_cast<int>(i);
  }
  nwc_toolkit::MultikeySort(pairs.begin(), pairs.end(), 0, PairKeyHandler());

  std::vector<int> converter(pairs.size());
  for (std::size_t i = 0; i < converter.size(); ++i) {
    converter[pairs[i].second] = i;
  }

  for (std::size_t i = 0; i < token_freqs.size(); ++i) {
    if (token_freqs[i] >= 0) {
      token_freqs[i] = converter[token_freqs[i]];
    }
  }

  std::vector<const int *> keys;
  keys.reserve(num_keys);
  for (std::size_t i = 1; i < token_freqs.size(); ++i) {
    if (token_freqs[i - 1] < 0) {
      keys.push_back(&token_freqs[i]);
    }
  }
  nwc_toolkit::MultikeySort(keys.begin(), keys.end(), 0, IntKeyHandler());

  for (std::size_t i = 0; i < token_freqs.size(); ++i) {
    if (token_freqs[i] >= 0) {
      token_freqs[i] = pairs[token_freqs[i]].second;
    }
  }

  char freq_buf[16];
  nwc_toolkit::StringBuilder result_buf;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    std::size_t index = 0;
    result_buf.Append(token_map_[keys[i][index]]);
    while (keys[i][++index] >= 0) {
      result_buf.Append(' ').Append(token_map_[keys[i][index]]);
    }
    int length = std::sprintf(freq_buf, "\t%d\n", -keys[i][index]);
    result_buf.Append(freq_buf, length);
    if (result_buf.length() > RESULT_BUF_LENGTH_THRESHOLD) {
      if (!output_file->Write(result_buf.str())) {
        return false;
      }
      result_buf.Clear();
    }
  }
  if (!output_file->Write(result_buf.str())) {
    return false;
  }
  return true;
}

bool NgramCounter::FlushWithoutSort(OutputFile *output_file) {
  nwc_toolkit::TokenTrieTracer token_trie_tracer;
  token_trie_tracer.Trace(token_trie_);

  nwc_toolkit::StringBuilder result_buf;
  char freq_buf[16];

  std::vector<int> tokens;
  int freq;

  while (token_trie_tracer.Next(&tokens, &freq)) {
    result_buf.Append(token_map_[tokens[0]]);
    for (std::size_t j = 1; j < tokens.size(); ++j) {
      result_buf.Append(' ').Append(token_map_[tokens[j]]);
    }
    int length = std::sprintf(freq_buf, "\t%d\n", freq);
    result_buf.Append(freq_buf, length);
    if (result_buf.length() > RESULT_BUF_LENGTH_THRESHOLD) {
      if (!output_file->Write(result_buf.str())) {
        return false;
      }
      result_buf.Clear();
    }
    tokens.clear();
  }
  if (!output_file->Write(result_buf.str())) {
    return false;
  }
  return true;
}

}  // namespace nwc_toolkit
