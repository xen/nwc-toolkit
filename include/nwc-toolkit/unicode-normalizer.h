// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_UNICODE_NORMALIZER_H_
#define NWC_TOOLKIT_UNICODE_NORMALIZER_H_

#include "./string-builder.h"

namespace nwc_toolkit {

class UnicodeNormalizer {
 public:
  // C = Composition, D = Comonposition, K = Compatibility.
  enum NormalizationForm {
    DEFAULT_FORM,  // ICU's default normalization form is NFC.
    NFC,
    NFKC,
    NFD,
    NFKD
  };

  // ICU replaces illegal input with U+FFFDs in default, and the U+FFFDs are
  // removed in Normalize() if `REMOVE_REPLACEMENT_CHARACTER' is given.
  enum IllegalInputHandler {
    KEEP_REPLACEMENT_CHARACTERS,
    REMOVE_REPLACEMENT_CHARACTERS,
    DEFAULT_HANDLER = KEEP_REPLACEMENT_CHARACTERS
  };

  static bool Normalize(NormalizationForm form,
      const String &src, StringBuilder *dest) {
    return Normalize(form, KEEP_REPLACEMENT_CHARACTERS, src, dest);
  }
  static bool Normalize(NormalizationForm form,
      IllegalInputHandler illegal_input_handler,
      const String &src, StringBuilder *dest);

 private:
  // Disallows object creation.
  UnicodeNormalizer();
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_UNICODE_NORMALIZER_H_
