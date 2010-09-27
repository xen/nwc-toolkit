// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CHARACTER_ENCODING_H_
#define NWC_TOOLKIT_CHARACTER_ENCODING_H_

#include "./string-builder.h"

namespace nwc_toolkit {

class CharacterEncoding {
 public:
  CharacterEncoding() {}
  ~CharacterEncoding() {}

  static bool Convert(const String &src_code, const String &src,
      const String &dest_code, StringBuilder *dest);

  static bool DetectFromBOM(const String &str, StringBuilder *encoding);
  static bool DetectFromResponseHead(const String &str,
      StringBuilder *encoding);
  static bool DetectFromHtml(const String &str, StringBuilder *encoding);
  static bool DetectFromXml(const String &str, StringBuilder *encoding);

 private:
  // Disallows copies.
  CharacterEncoding(const CharacterEncoding &);
  CharacterEncoding &operator=(const CharacterEncoding &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CHARACTER_ENCODING_H_
