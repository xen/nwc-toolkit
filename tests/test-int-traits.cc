// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/int-traits.h>

int main() {
  assert(nwc_toolkit::IntTraits<char>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<signed char>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<unsigned char>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<short>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<unsigned short>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<int>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<unsigned int>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<long>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<unsigned long>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<long long>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<unsigned long long>::Tag::is_int() == true);
  assert(nwc_toolkit::IntTraits<wchar_t>::Tag::is_int() == true);

  assert(nwc_toolkit::IntTraits<float>::Tag::is_int() == false);
  assert(nwc_toolkit::IntTraits<double>::Tag::is_int() == false);

  assert(nwc_toolkit::IntTraits<char *>::Tag::is_int() == false);
  assert(nwc_toolkit::IntTraits<const char *>::Tag::is_int() == false);

  return 0;
}
