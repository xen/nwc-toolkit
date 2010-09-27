// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/char-type.h>

#define NWC_TOOLKIT_TEST_CHAR_TYPE(cs_name, std_name) \
  do { \
    for (int c = 0; c < 256; ++c) { \
      assert(nwc_toolkit::cs_name()(c) == (std::std_name(c) != 0)); \
    } \
  } while (false)

int main() {
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsAlnum, isalnum);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsAlpha, isalpha);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsCntrl, iscntrl);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsDigit, isdigit);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsGraph, isgraph);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsLower, islower);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsPrint, isprint);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsPunct, ispunct);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsSpace, isspace);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsUpper, isupper);
  NWC_TOOLKIT_TEST_CHAR_TYPE(IsXdigit, isxdigit);

  return 0;
}
