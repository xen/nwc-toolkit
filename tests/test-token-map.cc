// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/token-map.h>

int main() {
  nwc_toolkit::TokenMap token_map;

  assert(token_map.num_tokens() == 0);

  assert(token_map.Insert("abc") == 0);
  assert(token_map.Insert("bcd") == 1);
  assert(token_map.Insert("abc") == 0);
  assert(token_map.Insert("cde") == 2);

  assert(token_map.num_tokens() == 3);

  assert(token_map["abc"] == 0);
  assert(token_map["bcd"] == 1);
  assert(token_map["cde"] == 2);

  assert(token_map[0] == "abc");
  assert(token_map[1] == "bcd");
  assert(token_map[2] == "cde");

  token_map.Clear();

  assert(token_map.num_tokens() == 0);

  assert(token_map.Insert("abc") == 0);
  assert(token_map.Insert("bcd") == 1);
  assert(token_map.Insert("abc") == 0);
  assert(token_map.Insert("cde") == 2);

  return 0;
}
