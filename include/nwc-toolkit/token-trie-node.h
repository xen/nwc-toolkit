// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_TOKEN_TRIE_NODE_H_
#define NWC_TOOLKIT_TOKEN_TRIE_NODE_H_

namespace nwc_toolkit {

class TokenTrieNode {
 public:
  enum {
    INVALID_NODE_ID = -1,
    INVALID_TOKEN_ID = -1
  };

  TokenTrieNode()
      : prev_node_id_(INVALID_NODE_ID),
        token_id_(INVALID_TOKEN_ID),
        freq_(0) {}

  int prev_node_id() const {
    return prev_node_id_;
  }
  int token_id() const {
    return token_id_;
  }
  int freq() const {
    return freq_;
  }

  void set_prev_node_id(int value) {
    prev_node_id_ = value;
  }
  void set_token_id(int value) {
    token_id_ = value;
  }
  void set_freq(int value) {
    freq_ = value;
  }

  void Clear() {
    prev_node_id_ = INVALID_NODE_ID;
    token_id_ = INVALID_TOKEN_ID;
    freq_ = 0;
  }

 private:
  int prev_node_id_;
  int token_id_;
  int freq_;

  // Copyable.
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_TOKEN_TRIE_NODE_H_
