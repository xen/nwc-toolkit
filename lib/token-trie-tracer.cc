// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/token-trie-tracer.h>

#include <algorithm>

namespace nwc_toolkit {

TokenTrieTracer::TokenTrieTracer()
    : token_trie_(NULL),
      table_(),
      current_node_id_(0),
      next_node_id_(0) {}

void TokenTrieTracer::Clear() {
  token_trie_ = NULL;
  std::vector<unsigned int>().swap(table_);
  current_node_id_ = 0;
  next_node_id_ = 0;
}

void TokenTrieTracer::Trace(const TokenTrie &token_trie) {
  Clear();
  token_trie_ = &token_trie;
  table_.resize((token_trie.table_size() + NUM_BITS_PER_INT - 1)
      / NUM_BITS_PER_INT, 0);
  current_node_id_ = TokenTrie::ROOT_NODE_ID;
  next_node_id_ = TokenTrie::ROOT_NODE_ID + 1;
}

bool TokenTrieTracer::Next(std::vector<int> *token_ids, int *freq) {
  if (RestoreFromCurrentNode(token_ids, freq)) {
    return true;
  }
  while (next_node_id_ < static_cast<int>(token_trie_->table_size())) {
    if (IsVisited(next_node_id_) || (token_trie_->node(
        next_node_id_).prev_node_id() == TokenTrie::INVALID_NODE_ID)) {
      ++next_node_id_;
      continue;
    }
    current_node_id_ = next_node_id_;
    ++next_node_id_;
    return RestoreFromCurrentNode(token_ids, freq);
  }
  return false;
}

bool TokenTrieTracer::RestoreFromCurrentNode(
    std::vector<int> *token_ids, int *freq) {
  if (current_node_id_ == TokenTrie::ROOT_NODE_ID) {
    return false;
  }
  int node_id = current_node_id_;
  table_[node_id / NUM_BITS_PER_INT] |= 1U << (node_id % NUM_BITS_PER_INT);

  current_node_id_ = token_trie_->node(node_id).prev_node_id();
  if (IsVisited(current_node_id_)) {
    current_node_id_ = TokenTrie::ROOT_NODE_ID;
  }

  std::size_t count = 0;
  *freq = token_trie_->node(node_id).freq();
  do {
    token_ids->push_back(token_trie_->node(node_id).token_id());
    node_id = token_trie_->node(node_id).prev_node_id();
    ++count;
  } while (node_id != TokenTrie::ROOT_NODE_ID);
  std::reverse(token_ids->end() - count, token_ids->end());
  return true;
}

}  // namespace nwc_toolkit
