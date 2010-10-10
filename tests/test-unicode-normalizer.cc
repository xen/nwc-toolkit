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

  dest.Clear();

  assert(nwc_toolkit::UnicodeNormalizer::Normalize(
      nwc_toolkit::UnicodeNormalizer::NFKC, "\xFF\xFF", &dest));
  assert(dest.str() == "\xEF\xBF\xBD\xEF\xBF\xBD");

  dest.Clear();

  assert(nwc_toolkit::UnicodeNormalizer::Normalize(
      nwc_toolkit::UnicodeNormalizer::NFKC,
      nwc_toolkit::UnicodeNormalizer::REMOVE_REPLACEMENT_CHARACTERS,
      "\xFF\xFF", &dest));
  assert(dest.str().is_empty());

  return 0;
}
