// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/unicode-normalizer.h>

int main() {
  nwc_toolkit::StringBuilder dest;

  assert(nwc_toolkit::UnicodeNormalizer::Normalize(
      nwc_toolkit::UnicodeNormalizer::NFKC, "ひらがな", &dest));
  assert(dest.str() == "ひらがな");

  assert(nwc_toolkit::UnicodeNormalizer::Normalize(
      nwc_toolkit::UnicodeNormalizer::NFKC, " 半角ｶﾀｶﾅ ｶﾞ", &dest));
  assert(dest.str() == "ひらがな 半角カタカナ ガ");

  dest.Clear();

  assert(nwc_toolkit::UnicodeNormalizer::Normalize(
      nwc_toolkit::UnicodeNormalizer::NFKC, "ＡＢＣ　１２３", &dest));
  assert(dest.str() == "ABC 123");

  assert(nwc_toolkit::UnicodeNormalizer::Normalize(
      nwc_toolkit::UnicodeNormalizer::NFKC, "… ．？！ ㌧㌦", &dest));
  assert(dest.str() == "ABC 123... .?! トンドル");

  return 0;
}
