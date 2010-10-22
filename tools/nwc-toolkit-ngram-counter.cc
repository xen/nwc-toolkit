// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include <nwc-toolkit/input-file.h>
#include <nwc-toolkit/multikey-sort.h>
#include <nwc-toolkit/output-file.h>
#include <nwc-toolkit/token-map.h>
#include <nwc-toolkit/token-trie.h>
#include <nwc-toolkit/token-trie-tracer.h>

#define NWC_TOOLKIT_ERROR(fmt, ...) \
  error_at_line(-(__LINE__), errno, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

namespace {

enum MaxNumTokens {
  MIN_MAX_NUM_TOKENS = 1,
  MAX_MAX_NUM_TOKENS = 32,
  DEFAULT_MAX_NUM_TOKENS = 7
};

enum MaxMemusage {
  MIN_MAX_MEMUSAGE = 64,
  DEFAULT_MAX_MEMUSAGE = 1024,
  MAX_MAX_MEMUSAGE = 256 * 1024
};

enum MaxNumOutputFiles {
  MIN_MAX_NUM_OUTPUT_FILES = 1,
  DEFAULT_MAX_NUM_OUTPUT_FILES = 100,
  MAX_MAX_NUM_OUTPUT_FILES = 10000
};

enum InputFileFormat {
  WAKATI_FORMAT,
  MECAB_FORMAT,
  CHASEN_FORMAT,
  DEFAULT_FORMAT = WAKATI_FORMAT
};

enum {
  RESULT_LENGTH_FLUSH_THRESHOLD = (1 << 16) - (1 << 10)
};

enum {
  START_TOKEN_ID = 0,
  END_TOKEN_ID = 1
};

const char * const DEFAULT_OUTPUT_FILE_PREFIX = "ngms-%Y%m%d-%H%M%S";

InputFileFormat input_file_format = DEFAULT_FORMAT;
int max_num_tokens = DEFAULT_MAX_NUM_TOKENS;
std::size_t max_mem_usage = DEFAULT_MAX_MEMUSAGE << 20;
int max_num_output_files = DEFAULT_MAX_NUM_OUTPUT_FILES;
nwc_toolkit::String output_file_prefix;
nwc_toolkit::String output_file_extension = "gz";
bool is_boundary_count_enabled = false;
bool is_sorting_enabled = false;
bool is_help_mode = false;

int ParseIntegerValue(const char *arg, long min_value, long max_value) {
  char *end_of_value;
  long value = strtol(optarg, &end_of_value, 10);
  if (*end_of_value != '\0' || (value < min_value) || (value > max_value)) {
    return -1;
  }
  return static_cast<int>(value);
}

void ParseOptions(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "tokens", 1, NULL, 't' },
    { "memusage", 1, NULL, 'u' },
    { "wakati", 0, NULL, 'w' },
    { "mecab", 0, NULL, 'm' },
    { "chasen", 0, NULL, 'c' },
    { "boundary", 0, NULL, 'b' },
    { "sort", 0, NULL, 's' },
    { "prefix", 1, NULL, 'p' },
    { "extension", 1, NULL, 'e' },
    { "files", 1, NULL, 'f' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, '\0' }
  };

  int value;
  while ((value = ::getopt_long(argc, argv,
      "t:u:wmcbsp:s:f:h", long_options, NULL)) != -1) {
    switch (value) {
      case 't': {
        max_num_tokens = ParseIntegerValue(optarg,
            MIN_MAX_NUM_TOKENS, MAX_MAX_NUM_TOKENS);
        if (max_num_tokens < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
        break;
      }
      case 'u': {
        max_mem_usage = ParseIntegerValue(optarg,
            MIN_MAX_MEMUSAGE, MAX_MAX_MEMUSAGE);
        if (max_mem_usage < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
        if (max_mem_usage > (std::numeric_limits<std::size_t>::max() >> 20)) {
          NWC_TOOLKIT_ERROR("too large memory usage: `%c', %s", value, optarg);
        }
        max_mem_usage <<= 20;
        break;
      }
      case 'w': {
        input_file_format = WAKATI_FORMAT;
        break;
      }
      case 'm': {
        input_file_format = MECAB_FORMAT;
        break;
      }
      case 'c': {
        input_file_format = CHASEN_FORMAT;
        break;
      }
      case 'b': {
        is_boundary_count_enabled = true;
        break;
      }
      case 's': {
        is_sorting_enabled = true;
        break;
      }
      case 'p': {
        output_file_prefix = optarg;
        break;
      }
      case 'e': {
        output_file_extension = optarg;
        break;
      }
      case 'f': {
        max_num_output_files = ParseIntegerValue(optarg,
            MIN_MAX_NUM_OUTPUT_FILES, MAX_MAX_NUM_OUTPUT_FILES);
        if (max_num_output_files < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
        break;
      }
      case 'h': {
        is_help_mode = true;
        break;
      }
      default: {
        NWC_TOOLKIT_ERROR("invalid option");
      }
    }
  }
}

void PrintHelp(const char *command) {
  std::cerr << "Usage: " << command << " [OPTION]... [FILE]...\n\n"
      "Options:\n"
      "  -t, --tokens=[N: " << MIN_MAX_NUM_TOKENS
      << '-' << MAX_MAX_NUM_TOKENS << "]\n"
      "                  limit the number of tokens to N-grams (default: 7)\n"
      "  -u, --memusage=[N: " << MIN_MAX_MEMUSAGE
      << '-' << MAX_MAX_MEMUSAGE << "]\n"
      "                  limit the memory usage to N MiB (default: 1024)\n"
      "  -w, --wakati    input wakati-formatted text (default)\n"
      "  -m, --mecab     input mecab-formatted text\n"
      "  -c, --chasen    input chasen-formatted text\n"
      "  -b, --boundary  count sentence boundaries as <S> and </S>\n"
      "  -s, --sort      sort result\n"
      "  -p, --prefix=[S]     set the prefix of output files\n"
      "                       (default: "<< DEFAULT_OUTPUT_FILE_PREFIX << ")\n"
      "  -e, --extension=[S]  set the extension of output files (default: gz)\n"
      "                       gz, bz2, or xz forces compression of result\n"
      "  -f, --files=[N: " << MIN_MAX_NUM_OUTPUT_FILES
      << '-' << MAX_MAX_NUM_OUTPUT_FILES << "]\n"
      "                  limit the number of output files to N (default: 100)\n"
      "  -h, --help      print this help\n"
      << std::flush;
}

int output_file_id = 0;
nwc_toolkit::TokenMap token_map;
nwc_toolkit::TokenTrie token_trie;

std::time_t start_time = std::time(NULL);
long long token_count = 0;
long long sentence_count = 0;

void PrintProgress() {
  std::cerr << '\r' << sentence_count << '/' << token_count
      << " (x" << std::fixed << std::setprecision(2)
      << ((sentence_count != 0) ? (1.0 * token_count / sentence_count) : 0.0)
      << "), " << token_map.num_tokens()
      << ", " << token_trie.num_nodes()
      << "/" << token_trie.total_length()
      << " (x" << std::fixed << std::setprecision(2)
      << ((token_trie.total_length() != 0)
      ? (1.0 * token_trie.total_length() / token_trie.num_nodes()) : 0.0)
      << ") (" << (std::time(NULL) - start_time) << "sec)";
}

// This function must not be called more than once.
void GenerateDefaultOutputFilePrefix() {
  static char static_buf[32];
  std::time_t current_time = std::time(NULL);
  std::strftime(static_buf, sizeof(static_buf), DEFAULT_OUTPUT_FILE_PREFIX,
      std::localtime(&current_time));
  output_file_prefix = static_buf;
}

void OpenNextOutputFile(nwc_toolkit::OutputFile *output_file) {
  if (output_file_id >= max_num_output_files) {
    NWC_TOOLKIT_ERROR("too many output files: %d", output_file_id + 1);
  }
  std::stringstream stream;
  stream << output_file_prefix;
  stream << '.' << std::setw(4) << std::setfill('0') << output_file_id;
  if (!output_file_extension.is_empty()) {
    stream << '.' << output_file_extension;
  }
  std::string output_file_name = stream.str();
  std::cerr << "output: " << output_file_name << std::endl;
  if (!output_file->Open(output_file_name.c_str())) {
    NWC_TOOLKIT_ERROR("failed to open file: %s", output_file_name.c_str());
  }
  ++output_file_id;
}

void ClearMapAndTrie() {
  token_map.Clear();
  if (is_sorting_enabled) {
    token_map.Insert("<S>");
    token_map.Insert("</S>");
  }
  token_trie.Clear();
}

bool ReadWakatiTokens(nwc_toolkit::InputFile *input_file,
    std::vector<int> *tokens) {
  std::size_t num_tokens = tokens->size();
  nwc_toolkit::String line;
  while (input_file->ReadLine(&line)) {
    line = line.StripRight();
    while (!line.is_empty()) {
      nwc_toolkit::String delim = line.FindFirstOf(' ');
      nwc_toolkit::String token(line.begin(), delim.begin());
      if (!token.is_empty()) {
        tokens->push_back(token_map.Insert(token));
      }
      line.set_begin(delim.end());
    }
    // Skips empty sentences.
    if (tokens->size() > num_tokens) {
      return true;
    }
  }
  return false;
}

bool ReadMecabTokens(nwc_toolkit::InputFile *input_file,
    std::vector<int> *tokens) {
  std::size_t num_tokens = tokens->size();
  nwc_toolkit::String line;
  while (input_file->ReadLine(&line)) {
    line = line.StripRight();
    if (line == "EOS") {
      // Skips empty sentences.
      if (tokens->size() > num_tokens) {
        return true;
      } else {
        continue;
      }
    }
    nwc_toolkit::String delim = line.FindFirstOf('\t');
    nwc_toolkit::String token(line.begin(), delim.begin());
    if (!token.is_empty()) {
      tokens->push_back(token_map.Insert(token));
    }
  }
  return false;
}

bool ReadChasenTokens(nwc_toolkit::InputFile *input_file,
    std::vector<int> *tokens) {
  return ReadMecabTokens(input_file, tokens);
}

bool ReadTokens(nwc_toolkit::InputFile *input_file,
    std::vector<int> *tokens) {
  tokens->clear();
  if (is_boundary_count_enabled) {
    tokens->push_back(START_TOKEN_ID);
  }
  switch (input_file_format) {
    case WAKATI_FORMAT: {
      if (!ReadWakatiTokens(input_file, tokens)) {
        return false;
      }
      break;
    }
    case MECAB_FORMAT: {
      if (!ReadMecabTokens(input_file, tokens)) {
        return false;
      }
      break;
    }
    case CHASEN_FORMAT: {
      if (!ReadChasenTokens(input_file, tokens)) {
        return false;
      }
      break;
    }
    default: {
      NWC_TOOLKIT_ERROR("invalid input file format: %d", input_file_format);
    }
  }
  if (is_boundary_count_enabled) {
    tokens->push_back(END_TOKEN_ID);
  }
  return true;
}

bool ToBeFlushed() {
  if (token_trie.max_freq() >= (std::numeric_limits<int>::max() / 2)) {
    return true;
  }

  if (!token_trie.is_empty() &&
      ((1.0 * token_trie.num_nodes() / token_trie.table_size()) >= 0.8)) {
    return true;
  }

  if (is_sorting_enabled) {
    std::size_t list_mem_usage = token_trie.total_length() * sizeof(int)
        + token_trie.num_nodes() * sizeof(int);
    if ((list_mem_usage + token_trie.memory_usage()) > max_mem_usage) {
      return true;
    }
  }
  return false;
}

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

void FlushNgramsWithSort(nwc_toolkit::OutputFile *output_file) {
  std::vector<int> token_freqs;
  token_freqs.reserve(token_trie.total_length() + token_trie.num_nodes());
  token_freqs.push_back(-1);

  nwc_toolkit::TokenTrieTracer token_trie_tracer;
  token_trie_tracer.Trace(token_trie);
  int freq;
  while (token_trie_tracer.Next(&token_freqs, &freq)) {
    token_freqs.push_back(-freq);
  }
  token_trie_tracer.Clear();

  std::size_t num_keys = token_trie.num_nodes() - 1;
  token_trie.Clear();

  std::vector<std::pair<nwc_toolkit::String, int> > pairs;
  pairs.resize(token_map.num_tokens());
  for (std::size_t i = 0; i < pairs.size(); ++i) {
    pairs[i].first = token_map[i];
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
  nwc_toolkit::StringBuilder result;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    std::size_t index = 0;
    result.Append(token_map[keys[i][index]]);
    while (keys[i][++index] >= 0) {
      result.Append(' ').Append(token_map[keys[i][index]]);
    }
    int length = std::sprintf(freq_buf, "\t%d\n", -keys[i][index]);
    result.Append(freq_buf, length);
    if (result.length() > RESULT_LENGTH_FLUSH_THRESHOLD) {
      output_file->Write(result.str());
      result.Clear();
    }
  }
  if (!result.is_empty()) {
    output_file->Write(result.str());
  }
}

void FlushNgramsWithoutSort(nwc_toolkit::OutputFile *output_file) {
  nwc_toolkit::TokenTrieTracer token_trie_tracer;
  token_trie_tracer.Trace(token_trie);

  nwc_toolkit::StringBuilder result;
  char freq_buf[16];

  std::vector<int> tokens;
  int freq;

  while (token_trie_tracer.Next(&tokens, &freq)) {
    result.Append(token_map[tokens[0]]);
    for (std::size_t j = 1; j < tokens.size(); ++j) {
      result.Append(' ').Append(token_map[tokens[j]]);
    }
    int length = std::sprintf(freq_buf, "\t%d\n", freq);
    result.Append(freq_buf, length);
    if (result.length() > RESULT_LENGTH_FLUSH_THRESHOLD) {
      output_file->Write(result.str());
      result.Clear();
    }
    tokens.clear();
  }

  if (!result.is_empty()) {
    output_file->Write(result.str());
  }
}

void FlushNgrams() {
  PrintProgress();
  std::cerr << std::endl;

  nwc_toolkit::OutputFile output_file;
  OpenNextOutputFile(&output_file);

  if (is_sorting_enabled) {
    FlushNgramsWithSort(&output_file);
  } else {
    FlushNgramsWithoutSort(&output_file);
  }

  ClearMapAndTrie();
}

void CountNgrams(nwc_toolkit::InputFile *input_file) {
  std::vector<int> tokens;
  while (ReadTokens(input_file, &tokens)) {
    if (tokens.empty()) {
      NWC_TOOLKIT_ERROR("unexpected empty sentence");
    }
    token_trie.Insert(&tokens[0], tokens.size());
    if (ToBeFlushed()) {
      FlushNgrams();
    }

    token_count += tokens.size();
    ++sentence_count;
    if ((sentence_count % 10000) == 0) {
      PrintProgress();
    }
  }

  FlushNgrams();
  ++output_file_id;
}

}  // namespace

int main(int argc, char *argv[]) {
  ParseOptions(argc, argv);
  if (is_help_mode) {
    PrintHelp(argv[0]);
    return 0;
  }

  std::size_t token_trie_mem_usage = max_mem_usage;
  if (is_sorting_enabled) {
    token_trie_mem_usage = max_mem_usage / (164 + (13 * max_num_tokens)) * 100;
  }
  token_trie.Reset(max_num_tokens, token_trie_mem_usage);

  if (output_file_prefix.is_empty()) {
    GenerateDefaultOutputFilePrefix();
  }

  if (optind == argc) {
    nwc_toolkit::InputFile input_file;
    std::cerr << "input: (standard input)" << std::endl;
    if (!input_file.Open(NULL)) {
      NWC_TOOLKIT_ERROR("failed to open standard input");
    }
    CountNgrams(&input_file);
  }

  for (int i = optind; i < argc; ++i) {
    nwc_toolkit::String input_file_name = argv[i];
    std::cerr << "input: " << (input_file_name.is_empty()
        ? "(standard input)" : input_file_name) << std::endl;
    nwc_toolkit::InputFile input_file;
    if (!input_file.Open(input_file_name)) {
      NWC_TOOLKIT_ERROR("failed to open input file: %s",
          input_file_name.ptr());
    }
    CountNgrams(&input_file);
  }

  return 0;
}
