// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CHARACTER_ENCODING_H_
#define NWC_TOOLKIT_CHARACTER_ENCODING_H_

#include "./string-builder.h"

namespace nwc_toolkit {

class CharacterEncoding {
 public:
  static bool Convert(const String &src_code, const String &src,
      const String &dest_code, StringBuilder *dest);

  static bool DetectFromBOM(
      const String &str, StringBuilder *encoding);
  static bool DetectFromResponseHeader(
      const String &str, StringBuilder *encoding);
  static bool DetectFromHtmlHeader(
      const String &str, StringBuilder *encoding);
  static bool DetectFromXmlHeader(
      const String &str, StringBuilder *encoding);

 private:
  // Disallows object creation.
  CharacterEncoding();
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CHARACTER_ENCODING_H_
