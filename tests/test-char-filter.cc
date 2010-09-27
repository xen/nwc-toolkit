// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/char-filter.h>

int main() {
  for (int c = 0; c < 256; ++c) {
    assert(nwc_toolkit::ToLower()(c) == static_cast<char>(std::tolower(c)));
  }

  for (int c = 0; c < 256; ++c) {
    assert(nwc_toolkit::ToUpper()(c) == static_cast<char>(std::toupper(c)));
  }

  return 0;
}
