// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>
#include <cctype>

#include <nwc-toolkit/char-table.h>

int main() {
  static const nwc_toolkit::CharTable LOWER_TABLE("a-z");
  for (int c = 0; c < 256; ++c) {
    assert(LOWER_TABLE.Get(c) == (std::islower(c) != 0));
  }

  static const nwc_toolkit::CharTable UPPER_TABLE("Z-A");
  for (int c = 0; c < 256; ++c) {
    assert(UPPER_TABLE.Get(c) == (std::isupper(c) != 0));
  }

  static const nwc_toolkit::CharTable ALNUM_TABLE("0-9A-Za-z");
  for (int c = 0; c < 256; ++c) {
    assert(ALNUM_TABLE.Get(c) == (std::isalnum(c) != 0));
  }

  static const nwc_toolkit::CharTable ASCII_TABLE("\x7F-", 3);
  for (int c = 0; c < 256; ++c) {
    assert(ASCII_TABLE.Get(c) == (c < 0x80));
  }

  return 0;
}
