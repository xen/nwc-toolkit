// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_UNICODE_NORMALIZER_H_
#define NWC_TOOLKIT_UNICODE_NORMALIZER_H_

#include "./string-builder.h"

namespace nwc_toolkit {

class UnicodeNormalizer {
 public:
  enum NormalizationForm {
    DEFAULT_NORMALIZATION_FORM,
    NFC,
    NFKC,
    NFD,
    NFKD
  };

  static bool Normalize(NormalizationForm form,
      const String &src, StringBuilder *dest);

 private:
  // Disallows object creation.
  UnicodeNormalizer();
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_UNICODE_NORMALIZER_H_
