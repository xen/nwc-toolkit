// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/text-filter.h>

namespace nwc_toolkit {

void TextFilter::Filter(const String &src, StringBuilder *dest) {
  enum { MIN_COUNT = 6, MAX_COUNT = 1023 };
  enum { MIN_NIHONGO_PERCENT = 70 };
  enum { MIN_HIRAGANA_PERCENT = 5 };

  String avail = src;
  String sentence;
  while (ExtractSentence(avail, &avail, &sentence)) {
    std::size_t dest_length = dest->length();
    std::size_t count = 0;
    std::size_t nihongo_count = 0;
    std::size_t hiragana_count = 0;

    int code_point = 0;
    String character;
    while (ExtractCharacter(sentence, &sentence, &character, &code_point)) {
      if (IsControl(code_point)) {
        continue;
      } else if (IsSpace()(character[0])) {
        if ((dest->length() > dest_length) &&
            !IsSpace()((*dest)[dest->length() - 1])) {
          dest->Append(' ');
        }
      } else {
        if (++count > MAX_COUNT) {
          break;
        } else if (IsNihongo(code_point)) {
          ++nihongo_count;
          if (IsHiragana(code_point)) {
            ++hiragana_count;
          }
        }
        dest->Append(character);
      }
    }

    if ((count < MIN_COUNT) || (count > MAX_COUNT) ||
        ((100 * nihongo_count / count) < MIN_NIHONGO_PERCENT) ||
        ((100 * hiragana_count / count) < MIN_HIRAGANA_PERCENT)) {
      dest->Resize(dest_length);
    } else if (IsSpace()((*dest)[dest->length() - 1])) {
      (*dest)[dest->length() - 1] = '\n';
    } else {
      dest->Append('\n');
    }
  }
}

bool TextFilter::ExtractCharacter(const String &src,
    String *src_left, String *character, int *code_point) {
  if (src.is_empty()) {
    return false;
  }
  *code_point = static_cast<unsigned char>(src[0]);
  std::size_t length = 1;
  if (*code_point >= 0x80) {
    while (*code_point & 0x40) {
      ++length;
      *code_point <<= 1;
    }
    *code_point = (*code_point & 0x3F) >> (length - 1);
    for (std::size_t i = 1; i < length; ++i) {
      *code_point <<= 6;
      *code_point |= static_cast<unsigned char>(src[i]) & 0x3F;
    }
  }
  *character = src.SubString(0, length);
  *src_left = src.SubString(length);
  return true;
}

bool TextFilter::ExtractSentence(const String &src,
    String *src_left, String *sentence) {
  static const CharTable DELIM_TABLE("\r\n.!?\xE3\xEF");
  static const String MULTI_BYTE_DELIMS[] = {
    "\xE3\x80\x82",  // 。 IDEOGRAPHIC FULL STOP (U+3002)
    "\xEF\xBC\x81",  // ！ FULLWIDTH EXCLAMATION MARK (U+FF01)
    "\xEF\xBC\x8E",  // ． FULLWIDTH FULL STOP (U+FF0E)
    "\xEF\xBC\x9F"   // ？ FULLWIDTH QUESTION MARK (U+FF1F)
  };
  static const std::size_t NUM_MULTI_BYTE_DELIMS =
      sizeof(MULTI_BYTE_DELIMS) / sizeof(MULTI_BYTE_DELIMS[0]);

  *src_left = src;
  *sentence = src;
  while (!src_left->is_empty()) {
    String delim = src_left->FindFirstOf(DELIM_TABLE);
    if (delim.is_empty()) {
      src_left->set_begin(delim.end());
      *sentence = sentence->Strip();
      if (sentence->is_empty()) {
        return false;
      }
      return true;
    }

    src_left->set_begin(delim.begin());
    for (std::size_t i = 0; i < NUM_MULTI_BYTE_DELIMS; ++i) {
      if (src_left->StartsWith(MULTI_BYTE_DELIMS[i])) {
        *src_left = src_left->SubString(MULTI_BYTE_DELIMS[i].length());
        sentence->set_end(delim.begin());
        *sentence = sentence->Strip();
        if (!sentence->is_empty()) {
          return true;
        }
      }
    }

    *src_left = src_left->SubString(1);
    if (static_cast<unsigned char>(delim[0]) < 0x80) {
      sentence->set_end(delim.begin());
      *sentence = sentence->Strip();
      if (!sentence->is_empty()) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace nwc_toolkit
