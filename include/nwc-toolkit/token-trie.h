// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_TOKEN_TRIE_H_
#define NWC_TOOLKIT_TOKEN_TRIE_H_

#include <vector>

namespace nwc_toolkit {

class TokenTrie {
 public:
  TokenTrie(std::size_t max_depth, std::size_t mem_usage);
  ~TokenTrie() {
    Clear();
  }

  std::size_t max_depth() const {
    return max_depth_;
  }
  std::size_t mem_usage() const {
    return mem_usage_;
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

  bool is_empty() const { return num_nodes_ == 0; }

  void Clear();

  void Insert(const int *tokens, std::size_t num_tokens);

  bool Trace(int node_id, std::vector<int> *token_ids, int *freq);

 private:
  struct Node {
    int from;
    int token_id;
    int freq;
  };

  const std::size_t max_depth_;
  const std::size_t mem_usage_;
  std::vector<Node> table_;
  std::size_t num_nodes_;
  std::size_t total_length_;
  int max_freq_;

  enum {
    ROOT_NODE_ID = 0,
    INVALID_NODE_ID = -1,
    INVALID_TOKEN_ID = -1
  };

  void Init();

  std::pair<int, bool> InsertNode(int from, int token_id);
  int FindNext(int from, int token_id) const;

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
