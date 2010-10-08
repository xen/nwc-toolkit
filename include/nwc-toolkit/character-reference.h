// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CHARACTER_REFERENCE_H_
#define NWC_TOOLKIT_CHARACTER_REFERENCE_H_

#include "./string-builder.h"

namespace nwc_toolkit {

class CharacterReference {
 public:
  class EncodeQuotes {};

  // Special characters in the given string are replaced with character
  // references. If there are no such characters, Encode() returns false.
  // Otherwise, it returns true.
  static bool Encode(const String &src, StringBuilder *dest);
  static bool Encode(const String &src, StringBuilder *dest, EncodeQuotes);

  // Character references in the given string are replaced with the original
  // characters. If there are no character references, Decode() returns false,
  // Otherwise, it returns true.
  static bool Decode(const String &src, StringBuilder *dest);

 private:
  enum { MAX_CODE_POINT = 0x10FFFF };

  static bool DecodeCharacterReference(String *charref, StringBuilder *dest);

  static bool DecodeHexadecimalCharacterReference(
      String *charref, StringBuilder *dest);
  static bool DecodeDecimalCharacterReference(
      String *charref, StringBuilder *dest);
  static bool DecodeCharacterEntiryReference(
      String *charref, StringBuilder *dest);

  static void EncodeCodePoint(int code_point, StringBuilder *dest);

  // Disallows object creation.
  CharacterReference();
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CHARACTER_REFERENCE_H_
