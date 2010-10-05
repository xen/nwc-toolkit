// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/token-trie.h>

int main() {
  enum { MAX_DEPTH = 3, MEM_USAGE = 12 << 10 };

  nwc_toolkit::TokenTrie trie(MAX_DEPTH, MEM_USAGE);

  assert(trie.max_depth() == MAX_DEPTH);
  assert(trie.mem_usage() == MEM_USAGE);

  assert(trie.table_size() == 0);
  assert(trie.num_nodes() == 0);
  assert(trie.total_length() == 0);
  assert(trie.max_freq() == 0);

  assert(trie.is_empty());

  int tokens[] = { 0, 1, 2, 3, 4 };
  std::size_t num_tokens = sizeof(tokens) / sizeof(tokens[0]);

  trie.Insert(tokens, num_tokens);

  assert(trie.table_size() == MEM_USAGE / (sizeof(int) * 3));
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

  std::vector<int> token_ids;
  int freq;
  int count = 0;
  for (std::size_t i = 0; i < trie.table_size(); ++i) {
    int node_id = static_cast<int>(i);
    std::size_t original_size = token_ids.size();
    if (trie.Trace(node_id, &token_ids, &freq)) {
      std::size_t length = token_ids.size() - original_size;
      for (std::size_t j = 1; j < length; ++j) {
        assert(token_ids[token_ids.size() - j] >
            token_ids[token_ids.size() - (j + 1)]);
      }
      assert(freq > 0);
      ++count;
    }
  }
  assert(count == static_cast<int>(trie.num_nodes() - 1));
  assert(token_ids.size() == trie.total_length());

  trie.Clear();

  assert(trie.max_depth() == MAX_DEPTH);
  assert(trie.mem_usage() == MEM_USAGE);

  assert(trie.table_size() == 0);
  assert(trie.num_nodes() == 0);
  assert(trie.total_length() == 0);
  assert(trie.max_freq() == 0);

  assert(trie.is_empty());

  return 0;
}
