// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_TEXT_FILTER_H_
#define NWC_TOOLKIT_TEXT_FILTER_H_

#include "./string-builder.h"

namespace nwc_toolkit {

class TextFilter {
 public:
  static void Filter(const String &src, StringBuilder *dest);

  static bool ExtractCharacter(const String &src,
      String *src_left, String *character, int *code_point);

  static bool ExtractSentence(const String &src,
      String *src_left, String *sentence);

  static bool IsNihongo(int code_point) {
    return ((code_point >= 0x3040) && (code_point <= 0x30FF)) ||
      ((code_point >= 0x31F0) && (code_point <= 0x31FF)) ||
      ((code_point >= 0x3400) && (code_point <= 0x34BF)) ||
      ((code_point >= 0x4E00) && (code_point <= 0x9FFF)) ||
      ((code_point >= 0xF900) && (code_point <= 0xFAFF));
  }
  static bool IsHiragana(int code_point) {
    return (code_point >= 0x3040) && (code_point <= 0x309F);
  }
  static bool IsControl(int code_point);

 private:
  // Disallows object creation.
  TextFilter();
};

inline bool TextFilter::IsControl(int code_point) {
  switch (code_point) {
    case 0x200E:  // LEFT-TO-RIGHT MARK (U+200E)
    case 0x200F:  // RIGHT-TO-LEFT MARK (U+200F)
    case 0x202A:  // LEFT-TO-RIGHT EMBEDDING (U+202A)
    case 0x202B:  // RIGHT-TO-LEFT EMBEDDING (U+202B)
    case 0x202C:  // POP DIRECTIONAL FORMATTING (U+202C)
    case 0x202D:  // LEFT-TO-RIGHT OVERRIDE (U+202D)
    case 0x202E:  // RIGHT-TO-LEFT OVERRIDE (U+202E)
    case 0xFEFF:  // ZERO WIDTH NO-BREAK SPACE (U+FEFF) Byte order mark
    case 0xFFFD:  // REPLACEMENT CHARACTER (U+FFFD)
    case 0xFFFE: {  // Invalid code point
      return true;
    }
    default: {
      return false;
    }
  }
}

}  // namespace nwc_toolkit

#endif // NWC_TOOLKIT_TEXT_FILTER_H_
