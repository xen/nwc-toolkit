// Copyright 2010 Susumu Yata <syata@acm.org>

#include <algorithm>
#include <iostream>

#include <nwc-toolkit/token-trie.h>

namespace nwc_toolkit {

TokenTrie::TokenTrie(std::size_t max_depth, std::size_t mem_usage)
    : max_depth_(max_depth),
      mem_usage_(mem_usage),
      table_(),
      num_nodes_(0),
      total_length_(0),
      max_freq_(0) {}

void TokenTrie::Clear() {
  std::vector<Node>().swap(table_);
  num_nodes_ = 0;
  total_length_ = 0;
  max_freq_ = 0;
}

void TokenTrie::Insert(const int *tokens, std::size_t num_tokens)
{
  if (is_empty()) {
    Init();
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

bool TokenTrie::Trace(int node_id, std::vector<int> *token_ids, int *freq)
{
  if ((node_id == ROOT_NODE_ID) || (table_[node_id].from == INVALID_NODE_ID)) {
    return false;
  }
  *freq = table_[node_id].freq;
  std::size_t original_size = token_ids->size();
  do {
    token_ids->push_back(table_[node_id].token_id);
    node_id = table_[node_id].from;
  } while (node_id != ROOT_NODE_ID);
  std::reverse(token_ids->begin() + original_size, token_ids->end());
  return true;
}

void TokenTrie::Init() {
  static const Node INITIAL_NODE = {
    INVALID_NODE_ID,
    INVALID_TOKEN_ID,
    0
  };
  std::size_t table_size = mem_usage_ / sizeof(Node);
  table_.resize(table_size, INITIAL_NODE);
  table_[ROOT_NODE_ID].from = ROOT_NODE_ID;
  num_nodes_ = 1;
}

std::pair<int, bool> TokenTrie::InsertNode(int from, int token_id) {
  int next_node_id = FindNext(from, token_id);
  bool is_new_node = (table_[next_node_id].from == INVALID_NODE_ID);
  if (is_new_node) {
    table_[next_node_id].from = from;
    table_[next_node_id].token_id = token_id;
    ++num_nodes_;
  }
  if (++table_[next_node_id].freq > max_freq_) {
    max_freq_ = table_[next_node_id].freq;
  }
  return std::make_pair(next_node_id, is_new_node);
}

int TokenTrie::FindNext(int from, int token_id) const {
  int next = Hash(((0ULL + from) << 32) | token_id) % table_.size();
  while (table_[next].from != INVALID_NODE_ID) {
    if ((next != ROOT_NODE_ID) && (from == table_[next].from) &&
        (token_id == table_[next].token_id)) {
      break;
    }
    next = (next + 1) % table_.size();
  }
  return next;
}

}  // namespace nwc_toolkit
