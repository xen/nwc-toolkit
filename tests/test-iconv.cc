// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <iconv.h>

namespace {

bool TestEncoding(const char *encoding) {
  ::iconv_t id = ::iconv_open(encoding, encoding);
  if (id == (::iconv_t)-1) {
    return false;
  }
  ::iconv_close(id);
  return true;
}

}  // namespace

int main() {
  assert(TestEncoding("SHIFT_JIS"));
  assert(TestEncoding("CP932"));
  assert(TestEncoding("EUC-JP"));
  assert(TestEncoding("EUC-JP-MS"));
  assert(TestEncoding("ISO-2022-JP"));
  assert(TestEncoding("ISO-2022-JP-2"));
  assert(TestEncoding("UTF-8"));

  return 0;
}
