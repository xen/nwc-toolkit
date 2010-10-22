// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_TOKEN_TRIE_TRACER_H_
#define NWC_TOOLKIT_TOKEN_TRIE_TRACER_H_

#include <vector>

#include "./token-trie.h"

namespace nwc_toolkit {

class TokenTrieTracer {
 public:
  TokenTrieTracer();
  ~TokenTrieTracer() {
    Clear();
  }

  void Clear();

  void Trace(const TokenTrie &token_trie);
  bool Next(std::vector<int> *token_ids, int *freq);

 private:
  enum { NUM_BITS_PER_INT = sizeof(unsigned int) * 8 };

  const TokenTrie *token_trie_;
  std::vector<unsigned int> table_;
  int current_node_id_;
  int next_node_id_;

  bool IsVisited(std::size_t node_id) const {
    return (table_[node_id / NUM_BITS_PER_INT]
        & (1U << (node_id % NUM_BITS_PER_INT))) != 0;
  }

  bool RestoreFromCurrentNode(std::vector<int> *token_ids, int *freq);

  // Disallows copy and assignment.
  TokenTrieTracer(const TokenTrieTracer &);
  TokenTrieTracer &operator=(const TokenTrieTracer &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_TOKEN_TRIE_TRACER_H_
