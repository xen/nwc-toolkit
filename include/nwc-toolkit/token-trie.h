// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_TOKEN_TRIE_H_
#define NWC_TOOLKIT_TOKEN_TRIE_H_

#include <utility>
#include <vector>

#include "./token-trie-node.h"

namespace nwc_toolkit {

class TokenTrie {
 public:
  enum {
    MIN_MAX_DEPTH = 1,
    MIN_MEMORY_USAGE = 1 << 20,
    DEFAULT_MAX_DEPTH = 5,
    DEFAULT_MEMORY_USAGE = 256 << 20
  };

  enum {
    ROOT_NODE_ID = 0,
    INVALID_NODE_ID = TokenTrieNode::INVALID_NODE_ID,
    INVALID_TOKEN_ID = TokenTrieNode::INVALID_TOKEN_ID,
  };

  TokenTrie();
  ~TokenTrie() {
    Clear();
  }

  std::size_t max_depth() const {
    return max_depth_;
  }
  std::size_t memory_usage() const {
    return memory_usage_;
  }
  std::size_t table_size() const {
    return table_.size();
  }
  std::size_t num_nodes() const {
    return num_nodes_;
  }
  std::size_t total_length() const {
    return total_length_;
  }
  int max_freq() const {
    return max_freq_;
  }
  const TokenTrieNode &node(std::size_t node_id) const {
    return table_[node_id];
  }

  bool is_empty() const { return num_nodes_ == 0; }

  void Reset(std::size_t max_depth = 0, std::size_t memory_usage = 0);
  void Clear();

  void Insert(const int *tokens, std::size_t num_tokens);

 private:
  std::size_t max_depth_;
  std::size_t memory_usage_;
  std::vector<TokenTrieNode> table_;
  std::size_t num_nodes_;
  std::size_t total_length_;
  int max_freq_;

  void InitTable();

  std::pair<int, bool> InsertNode(int prev_node_id, int token_id);
  int FindNext(int prev_node_id, int token_id) const;

  static unsigned int Hash(unsigned long long x) {
    x = (~x) + (x << 18);
    x = x ^ (x >> 31);
    x = x * 21;
    x = x ^ (x >> 11);
    x = x + (x << 6);
    x = x ^ (x >> 22);
    return static_cast<unsigned int>(x);
  }

  // Disallows copy and assignment.
  TokenTrie(const TokenTrie &);
  TokenTrie &operator=(const TokenTrie &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_TOKEN_TRIE_H_
