// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/character-reference.h>

int main() {
  nwc_toolkit::StringBuilder dest;

  assert(!nwc_toolkit::CharacterReference::Encode("ABC", &dest) == true);
  assert(dest.str() == "ABC");

  assert(nwc_toolkit::CharacterReference::Encode("A<B>\"C\"",
      &dest) == true);
  assert(dest.str() == "ABCA&lt;B&gt;\"C\"");

  assert(nwc_toolkit::CharacterReference::Encode("P<L\"U\"S",
      &dest, nwc_toolkit::CharacterReference::EncodeQuotes()) == true);
  assert(dest.str() == "ABCA&lt;B&gt;\"C\"P&lt;L&quot;U&quot;S");

  dest.Clear();

  assert(!nwc_toolkit::CharacterReference::Decode("ABC", &dest) == true);
  assert(dest.str() == "ABC");

  assert(nwc_toolkit::CharacterReference::Decode("A&lt;B&gt;\"C\"",
      &dest) == true);
  assert(dest.str() == "ABCA<B>\"C\"");

  assert(nwc_toolkit::CharacterReference::Decode("P&lt;L&quot;U&quot;S",
      &dest) == true);
  assert(dest.str() == "ABCA<B>\"C\"P<L\"U\"S");

  return 0;
}
