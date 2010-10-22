// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/token-trie-node.h>

int main() {
  nwc_toolkit::TokenTrieNode node;

  assert(node.prev_node_id() == nwc_toolkit::TokenTrieNode::INVALID_NODE_ID);
  assert(node.token_id() == nwc_toolkit::TokenTrieNode::INVALID_TOKEN_ID);
  assert(node.freq() == 0);

  node.set_prev_node_id(1);
  node.set_token_id(2);
  node.set_freq(3);

  assert(node.prev_node_id() == 1);
  assert(node.token_id() == 2);
  assert(node.freq() == 3);

  node.Clear();

  assert(node.prev_node_id() == nwc_toolkit::TokenTrieNode::INVALID_NODE_ID);
  assert(node.token_id() == nwc_toolkit::TokenTrieNode::INVALID_TOKEN_ID);
  assert(node.freq() == 0);

  return 0;
}
