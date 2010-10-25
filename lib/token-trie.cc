// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/token-trie.h>

#include <limits>

namespace nwc_toolkit {

TokenTrie::TokenTrie()
    : max_depth_(DEFAULT_MAX_DEPTH),
      memory_usage_(DEFAULT_MEMORY_USAGE),
      table_(),
      num_nodes_(0),
      total_length_(0),
      max_freq_(0) {}

void TokenTrie::Reset(std::size_t max_depth, std::size_t memory_usage) {
  Clear();
  if (max_depth == 0) {
    max_depth = DEFAULT_MAX_DEPTH;
  } else if (max_depth < MIN_MAX_DEPTH) {
    max_depth = MIN_MAX_DEPTH;
  }
  if (memory_usage == 0) {
    memory_usage = DEFAULT_MEMORY_USAGE;
  } else if (memory_usage < MIN_MEMORY_USAGE) {
    memory_usage = MIN_MEMORY_USAGE;
  } else if ((memory_usage / sizeof(TokenTrieNode)) >
      static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    // TokenTrie uses a 32-bit signed integer for representing a node ID.
    // Thus the size of hash table must be less than 2^31.
    memory_usage = std::numeric_limits<int>::max() * sizeof(TokenTrieNode);
  }
  max_depth_ = max_depth;
  memory_usage_ = memory_usage;
}

void TokenTrie::Clear() {
  std::vector<TokenTrieNode>().swap(table_);
  num_nodes_ = 0;
  total_length_ = 0;
  max_freq_ = 0;
}

void TokenTrie::Insert(const int *tokens, std::size_t num_tokens) {
  if (is_empty()) {
    InitTable();
  }
  for (std::size_t i = 0; i < num_tokens; ++i) {
    int node_id = ROOT_NODE_ID;
    std::size_t end_id = i + max_depth_;
    if (end_id > num_tokens) {
      end_id = num_tokens;
    }
    for (std::size_t j = i; j < end_id; ++j) {
      std::pair<int, bool> result = InsertNode(node_id, tokens[j]);
      node_id = result.first;
      if (result.second) {
        total_length_ += j - i + 1;
      }
    }
  }
}

void TokenTrie::InitTable() {
  std::size_t table_size = memory_usage_ / sizeof(TokenTrieNode);
  table_.resize(table_size);
  table_[ROOT_NODE_ID].set_prev_node_id(ROOT_NODE_ID);
  num_nodes_ = 1;
}

std::pair<int, bool> TokenTrie::InsertNode(int prev_node_id, int token_id) {
  int next_node_id = FindNext(prev_node_id, token_id);
  bool is_new_node = (table_[next_node_id].prev_node_id() == INVALID_NODE_ID);
  if (is_new_node) {
    table_[next_node_id].set_prev_node_id(prev_node_id);
    table_[next_node_id].set_token_id(token_id);
    ++num_nodes_;
  }
  int freq = table_[next_node_id].freq() + 1;
  table_[next_node_id].set_freq(freq);
  if (freq > max_freq_) {
    max_freq_ = freq;
  }
  return std::make_pair(next_node_id, is_new_node);
}

int TokenTrie::FindNext(int prev_node_id, int token_id) const {
  int next = Hash(((0ULL + prev_node_id) << 32) | token_id) % table_.size();
  while (table_[next].prev_node_id() != INVALID_NODE_ID) {
    if ((next != ROOT_NODE_ID) &&
        (prev_node_id == table_[next].prev_node_id()) &&
        (token_id == table_[next].token_id())) {
      break;
    }
    next = (next + 1) % table_.size();
  }
  return next;
}

}  // namespace nwc_toolkit
