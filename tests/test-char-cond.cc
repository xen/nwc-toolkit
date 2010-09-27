// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>
#include <cstring>

#include <nwc-toolkit/char-cond.h>

int main() {
  static const nwc_toolkit::CondChar cond_char('X');
  for (int c = 0; c < 256; ++c) {
    assert(cond_char(c) == (c == 'X'));
  }

  static const nwc_toolkit::CondCString cond_cstring("ABC");
  assert(cond_cstring('\0') == false);
  for (int c = 1; c < 256; ++c) {
    assert(cond_cstring(c) == ((c >= 'A') && (c <= 'C')));
  }

  static const nwc_toolkit::CondString cond_string("ABC", 4);
  assert(cond_string('\0') == true);
  for (int c = 1; c < 256; ++c) {
    assert(cond_cstring(c) == ((c == '\0') || ((c >= 'A') && (c <= 'C'))));
  }

  static const nwc_toolkit::CondTable cond_table(
    nwc_toolkit::CharTable("-A-Z"));
  for (int c = 0; c < 256; ++c) {
    assert(cond_table(c) == ((c == '-') || ((c >= 'A') && (c <= 'Z'))));
  }

  return 0;
}
