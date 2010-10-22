// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/token-trie-tracer.h>

int main() {
  enum {
    MAX_DEPTH = 3,
    MEMORY_USAGE = nwc_toolkit::TokenTrie::MIN_MEMORY_USAGE * 2
  };

  nwc_toolkit::TokenTrie trie;
  trie.Reset(MAX_DEPTH, MEMORY_USAGE);

  int tokens[] = { 0, 1, 2, 3, 4 };
  std::size_t num_tokens = sizeof(tokens) / sizeof(tokens[0]);

  trie.Insert(tokens, num_tokens);
  trie.Insert(tokens, num_tokens);

  int tokens2[] = { 1, 3, 5 };
  std::size_t num_tokens2 = sizeof(tokens2) / sizeof(tokens2[0]);

  trie.Insert(tokens2, num_tokens2);

  assert(trie.num_nodes() == 17);
  assert(trie.total_length() == 30);
  assert(trie.max_freq() == 3);

  nwc_toolkit::TokenTrieTracer tracer;
  tracer.Trace(trie);

  std::vector<int> token_ids;
  int freq;
  int count = 0;
  std::size_t prev_size = 0;
  while (tracer.Next(&token_ids, &freq)) {
    for (std::size_t i = prev_size + 1; i < token_ids.size(); ++i) {
      assert(token_ids[i - 1] < token_ids[i]);
    }
    assert(freq > 0);

    prev_size = token_ids.size();
    ++count;
  }
  assert(count == static_cast<int>(trie.num_nodes() - 1));
  assert(token_ids.size() == trie.total_length());

  return 0;
}
