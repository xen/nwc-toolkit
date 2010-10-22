// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_NGRAM_COUNTER_H_
#define NWC_TOOLKIT_NGRAM_COUNTER_H_

#include <vector>

#include "./input-file.h"
#include "./output-file.h"
#include "./token-map.h"
#include "./token-trie.h"

namespace nwc_toolkit {

class NgramCounter {
 public:
  enum InputFormat {
    WAKATI_FORMAT,
    MECAB_FORMAT,
    CHASEN_FORMAT,
    DEFAULT_FORMAT = WAKATI_FORMAT
  };

  enum {
    MIN_MEMORY_LIMIT = 2 << 20,
    DEFAULT_MEMORY_LIMIT = 1024 << 20
  };

  enum {
    MIN_MAX_NGRAM_LENGTH = TokenTrie::MIN_MAX_DEPTH,
    DEFAULT_MAX_NGRAM_LENGTH = TokenTrie::DEFAULT_MAX_DEPTH
  };

  enum {
    START_TOKEN_ID = 0,
    END_TOKEN_ID = 1
  };

  enum {
    RESULT_BUF_LENGTH_THRESHOLD = (1 << 16) - (1 << 10)
  };

  NgramCounter();
  ~NgramCounter() {}

  InputFormat input_format() const {
    return input_format_;
  }
  bool with_boundary_count() const {
    return with_boundary_count_;
  }
  bool with_result_sort() const {
    return with_result_sort_;
  }
  std::size_t memory_limit() const {
    return memory_limit_;
  }
  std::size_t max_ngram_length() const {
    return token_trie_.max_depth();
  }
  long long token_count() const {
    return token_count_;
  }
  long long sentence_count() const {
    return sentence_count_;
  }

  void set_input_format(InputFormat value) {
    input_format_ = value;
  }
  void set_with_boundary_count(bool value) {
    with_boundary_count_ = value;
  }
  void set_with_result_sort(bool value) {
    with_result_sort_ = value;
  }

  bool is_empty() const {
    return token_trie_.is_empty();
  }

  // Reset() must be called after settings of other parameters.
  void Reset(std::size_t max_ngram_length = 0, std::size_t memory_limit = 0);
  void Clear();

  bool Count(InputFile *input_file);

  bool NeedsFlush();
  bool Flush(OutputFile *output_file);

 private:
  InputFormat input_format_;
  bool with_boundary_count_;
  bool with_result_sort_;
  std::size_t memory_limit_;
  nwc_toolkit::TokenMap token_map_;
  nwc_toolkit::TokenTrie token_trie_;
  std::vector<int> tokens_;
  long long token_count_;
  long long sentence_count_;

  bool ReadTokens(InputFile *input_file);
  bool ReadWakatiTokens(InputFile *input_file);
  bool ReadMecabTokens(InputFile *input_file);
  bool ReadChasenTokens(InputFile *input_file);

  bool FlushWithSort(OutputFile *output_file);
  bool FlushWithoutSort(OutputFile *output_file);

  // Disallows copy and assignment.
  NgramCounter(const NgramCounter &);
  NgramCounter &operator=(const NgramCounter &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_NGRAM_COUNTER_H_
