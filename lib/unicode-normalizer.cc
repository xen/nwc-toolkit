// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/unicode-normalizer.h>

#include <cstdint>

#include <unicode/normlzr.h>
#include <unicode/unistr.h>

namespace nwc_toolkit {
namespace {

::UNormalizationMode ConvertFormToMode(
    UnicodeNormalizer::NormalizationForm form) {
  switch (form) {
    case UnicodeNormalizer::DEFAULT_FORM: {
      return UNORM_DEFAULT;
    }
    case UnicodeNormalizer::NFC: {
      return UNORM_NFC;
    }
    case UnicodeNormalizer::NFKC: {
      return UNORM_NFKC;
    }
    case UnicodeNormalizer::NFD: {
      return UNORM_NFD;
    }
    case UnicodeNormalizer::NFKD: {
      return UNORM_NFKD;
    }
    default: {
      // Returns UNORM_NONE if the given `form' is undefined.
      return UNORM_NONE;
    }
  }
}

void ExtractStringFromUnicodeString(const UnicodeString &unistr,
    std::int32_t begin, std::int32_t end, StringBuilder *dest) {
  if (begin >= end) {
    return;
  }
  std::size_t offset = dest->length();

  // ::UnicodeString::extract() of ICU 4.2.1 is endless when the capacity of
  // its target buffer is 0. The next line expands the target buffer so as to
  // avoid this problem.
  dest->Reserve(offset + 1);

  dest->Resize(dest->size());
  std::size_t avail = dest->length() - offset;

  std::int32_t length = unistr.extract(begin, end - begin,
      dest->buf() + offset, avail, "UTF-8");
  dest->Resize(offset + length);
  if (static_cast<std::size_t>(length) > avail) {
    unistr.extract(begin, end - begin, dest->buf() + offset, length, "UTF-8");
  }
}

}  // namespace

bool UnicodeNormalizer::Normalize(NormalizationForm form,
    IllegalInputHandler illegal_input_handler,
    const String &src, StringBuilder *dest) {
  ::UNormalizationMode mode = ConvertFormToMode(form);
  if (mode == UNORM_NONE) {
    return false;
  }

  ::UnicodeString unicode_src(src.ptr(), src.length(), "UTF-8");
  ::UnicodeString unicode_dest;
  ::UErrorCode status = U_ZERO_ERROR;
  icu::Normalizer::normalize(unicode_src, mode, 0, unicode_dest, status);
  if (U_FAILURE(status)) {
    return false;
  }

  std::int32_t last_pos = 0;
  if (illegal_input_handler == REMOVE_REPLACEMENT_CHARACTERS) {
    for (std::int32_t pos = 0; pos < unicode_dest.length(); ++pos) {
      // ICU replaces illegal input with U+FFFDs, and the U+FFFDs are
      // skipped here if `REMOVE_REPLACEMENT_CHARACTERS' is given.
      if (unicode_dest[pos] == 0xFFFD) {
        ExtractStringFromUnicodeString(unicode_dest, last_pos, pos, dest);
        last_pos = pos + 1;
      }
    }
  }
  ExtractStringFromUnicodeString(unicode_dest,
      last_pos, unicode_dest.length(), dest);
  return true;
}

}  // namespace nwc_toolkit
