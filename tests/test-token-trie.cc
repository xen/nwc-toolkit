// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/token-trie.h>

int main() {
  enum {
    MAX_DEPTH = 3,
    MEMORY_USAGE = nwc_toolkit::TokenTrie::MIN_MEMORY_USAGE * 2
  };

  nwc_toolkit::TokenTrie trie;

  assert(trie.max_depth() == nwc_toolkit::TokenTrie::DEFAULT_MAX_DEPTH);
  assert(trie.memory_usage() == nwc_toolkit::TokenTrie::DEFAULT_MEMORY_USAGE);

  assert(trie.table_size() == 0);
  assert(trie.num_nodes() == 0);
  assert(trie.total_length() == 0);
  assert(trie.max_freq() == 0);

  trie.Reset(1, 1);

  assert(trie.max_depth() == nwc_toolkit::TokenTrie::MIN_MAX_DEPTH);
  assert(trie.memory_usage() == nwc_toolkit::TokenTrie::MIN_MEMORY_USAGE);

  trie.Reset();

  assert(trie.max_depth() == nwc_toolkit::TokenTrie::DEFAULT_MAX_DEPTH);
  assert(trie.memory_usage() == nwc_toolkit::TokenTrie::DEFAULT_MEMORY_USAGE);

  trie.Reset(MAX_DEPTH, MEMORY_USAGE);

  assert(trie.max_depth() == MAX_DEPTH);
  assert(trie.memory_usage() == MEMORY_USAGE);

  assert(trie.is_empty());

  int tokens[] = { 0, 1, 2, 3, 4 };
  std::size_t num_tokens = sizeof(tokens) / sizeof(tokens[0]);

  trie.Insert(tokens, num_tokens);

  assert(trie.table_size() == MEMORY_USAGE / (sizeof(int) * 3));
  assert(trie.num_nodes() == 13);
  assert(trie.total_length() == 22);
  assert(trie.max_freq() == 1);

  assert(trie.is_empty() == false);

  trie.Insert(tokens, num_tokens);

  assert(trie.num_nodes() == 13);
  assert(trie.total_length() == 22);
  assert(trie.max_freq() == 2);

  int tokens2[] = { 1, 3, 5 };
  std::size_t num_tokens2 = sizeof(tokens2) / sizeof(tokens2[0]);

  trie.Insert(tokens2, num_tokens2);

  assert(trie.num_nodes() == 17);
  assert(trie.total_length() == 30);
  assert(trie.max_freq() == 3);

  trie.Clear();

  assert(trie.max_depth() == MAX_DEPTH);
  assert(trie.memory_usage() == MEMORY_USAGE);

  assert(trie.table_size() == 0);
  assert(trie.num_nodes() == 0);
  assert(trie.total_length() == 0);
  assert(trie.max_freq() == 0);

  assert(trie.is_empty());

  return 0;
}
