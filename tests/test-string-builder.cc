// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/string-builder.h>

int main() {
  nwc_toolkit::StringBuilder builder;

  assert(builder.buf() != NULL);
  assert(builder.ptr() != NULL);
  assert(builder.length() == 0);
  assert(builder.size() == 0);

  assert(builder.is_empty() == true);

  builder.Append('a');
  assert(builder.str() == "a");
  assert(builder.is_empty() == false);

  builder.Append('b', nwc_toolkit::ToUpper());
  assert(builder.str() == "aB");

  builder.Append('C', nwc_toolkit::ToLower());
  assert(builder.str() == "aBc");

  builder.Append('0', nwc_toolkit::ToLower());
  assert(builder.str() == "aBc0");

  builder.Clear();

  assert(builder.buf() != NULL);
  assert(builder.length() == 0);
  assert(builder.size() == sizeof(builder.buf()));
  assert(builder.ptr() != NULL);

  assert(builder.is_empty() == true);

  builder.Append("aBc");
  assert(builder.str() == "aBc");

  builder.Append("DeF", nwc_toolkit::ToUpper());
  assert(builder.str() == "aBcDEF");

  builder.Append("gHi", nwc_toolkit::ToLower());
  assert(builder.str() == "aBcDEFghi");

  assert(builder.length() == 9);
  assert(builder.size() == 16);

  builder.Append("jKlMnO", 3);
  assert(builder.str() == "aBcDEFghijKl");

  builder.Append("MnOpQr", 3, nwc_toolkit::ToUpper());
  assert(builder.str() == "aBcDEFghijKlMNO");

  builder.Append("pQrStU", 3, nwc_toolkit::ToLower());
  assert(builder.str() == "aBcDEFghijKlMNOpqr");

  assert(builder.length() == 18);
  assert(builder.size() == 32);

  builder.Resize(31);
  assert(builder.length() == 31);
  assert(builder.size() == 32);

  builder.Resize(32);
  assert(builder.length() == 32);
  assert(builder.size() == 32);

  builder.Resize(33);
  assert(builder.length() == 33);
  assert(builder.size() == 64);

  builder.Resize(256);
  assert(builder.length() == 256);
  assert(builder.size() == 256);

  builder.Reserve(500);
  assert(builder.length() == 256);
  assert(builder.size() == 512);

  builder = "ABC";
  assert(builder.str() == "ABC");

  assert(builder.Assign("012345", 3).str() == "012");

  return 0;
}
