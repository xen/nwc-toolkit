// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/character-encoding.h>

#include <errno.h>

#include <iconv.h>

namespace nwc_toolkit {

bool CharacterEncoding::Convert(const String &src_code, const String &src,
    const String &dest_code, StringBuilder *dest) {
  StringBuilder src_code_clone, dest_code_clone;
  src_code_clone.Append(src_code).Append();
  dest_code_clone.Append(dest_code).Append();

  ::iconv_t iconv_desc = ::iconv_open(
      dest_code_clone.ptr(), src_code_clone.ptr());
  if (iconv_desc == reinterpret_cast<iconv_t>(-1))
    return false;

  char *in_buf = const_cast<char *>(src.ptr());
  std::size_t in_bytes_left = src.length();

  std::size_t original_dest_length = dest->length();
  std::size_t out_bytes_total = dest->length();
  dest->Resize(dest->size());

  char *out_buf = dest->buf() + out_bytes_total;
  std::size_t out_bytes_left = dest->length() - out_bytes_total;

  bool iconv_ok = true;
  while (iconv_ok) {
    std::size_t iconv_result = ::iconv(iconv_desc, &in_buf, &in_bytes_left,
        &out_buf, &out_bytes_left);
    if (iconv_result != static_cast<std::size_t>(-1)) {
      break;
    }

    switch (errno) {
      case E2BIG: {
        out_bytes_total = dest->length() - out_bytes_left;
        dest->Resize((dest->size() != 0) ? (dest->size() * 2) : 1);
        out_buf = dest->buf() + out_bytes_total;
        out_bytes_left = dest->length() - out_bytes_total;
        break;
      }
      case EILSEQ:
      case EINVAL: {
        iconv_ok = false;
        break;
      }
    }
  }

  if (iconv_ok) {
    out_bytes_total = dest->length() - out_bytes_left;
    dest->Resize(out_bytes_total);
  } else {
    dest->Resize(original_dest_length);
  }

  ::iconv_close(iconv_desc);
  return iconv_ok;
}

bool CharacterEncoding::DetectFromBOM(
    const String &str, StringBuilder *encoding) {
  static const char UTF32_LE_BOM[4] = { '\xFF', '\xFE', '\x00', '\x00' };
  static const char UTF32_BE_BOM[4] = { '\x00', '\x00', '\xFE', '\xFF' };

  encoding->Clear();
  if (str.StartsWith(UTF32_LE_BOM, sizeof(UTF32_LE_BOM))) {
    encoding->Assign("UTF-32LE");
  } else if (str.StartsWith(UTF32_BE_BOM, sizeof(UTF32_BE_BOM))) {
    encoding->Assign("UTF-32BE");
  } else if (str.StartsWith("\xFF\xFE")) {
    encoding->Assign("UTF-16LE");
  } else if (str.StartsWith("\xFE\xFF")) {
    encoding->Assign("UTF-16BE");
  } else if (str.StartsWith("\xEF\xBB\xBF")) {
    encoding->Assign("UTF-8");
  } else if (str.StartsWith("\x2B\x2F\x76")) {
    encoding->Assign("UTF-7");
  } else {
    return false;
  }
  return true;
}

bool CharacterEncoding::DetectFromResponseHeader(
    const String &str, StringBuilder *encoding) {
  static const String FIELD_NAME("Content-Type:");

  encoding->Clear();
  for (String avail = str; !avail.is_empty();
       avail.set_begin(avail.FindFirstOf('\n').end())) {
    if (avail.StartsWith(FIELD_NAME, ToLower())) {
      String field_value = avail.SubString(FIELD_NAME.length());
      field_value.set_end(field_value.FindFirstOf("\r\n").begin());
      String charset = field_value.Find("charset=", ToLower());
      charset.Assign(charset.end(), field_value.end());
      charset.set_end(charset.FindFirstOf(";").begin());
      charset = charset.Strip().Strip("'\"");
      if (!charset.is_empty()) {
        encoding->Assign(charset, ToUpper());
        return true;
      }
    }
  }
  return false;
}

bool CharacterEncoding::DetectFromHtmlHeader(
    const String &str, StringBuilder *encoding) {
  static const String COMMENT_START_MARK = "<!--";
  static const String COMMENT_END_MARK = "-->";

  static const String META_TAG_START_MARK = "<meta";

  static const CharTable CHARSET_DELIM_TABLE(" \t\r\n'\";");
  static const CharTable TAG_NAME_TABLE("A-Za-z/!?");

  encoding->Clear();
  for (String avail = str; !avail.is_empty(); ) {
    avail.set_begin(avail.FindFirstOf('<').begin());
    if (avail.StartsWith(COMMENT_START_MARK)) {
      avail.set_begin(avail.SubString(
          COMMENT_START_MARK.length()).Find(COMMENT_END_MARK).end());
    } else if (avail.StartsWith(META_TAG_START_MARK, ToLower())) {
      String tag_end = avail.SubString(
          META_TAG_START_MARK.length()).FindFirstOf('>');
      String tag(avail.begin() + 1, tag_end.begin());

      String charset_name = tag.Find("charset");
      String charset_value(charset_name.end(), tag.end());
      charset_value = charset_value.StripLeft();
      if (charset_value.StartsWith("=")) {
        charset_value = charset_value.SubString(1).StripLeft();
        if (!charset_value.is_empty() &&
            (charset_value[0] == '"' || charset_value[0] == '\'')) {
          char quote = charset_value[0];
          charset_value = charset_value.SubString(1);
          charset_value.set_end(charset_value.FindFirstOf(quote).begin());
          if (!charset_value.is_empty()) {
            encoding->Assign(charset_value, ToUpper());
            return true;
          }
        } else {
          charset_value.set_end(charset_value.FindFirstOf(
              CHARSET_DELIM_TABLE).begin());
          if (!charset_value.is_empty()) {
            encoding->Assign(charset_value, ToUpper());
            return true;
          }
        }
      }
      avail.set_begin(tag_end.end());
    } else if (avail.StartsWith("</head", ToLower()) ||
        avail.StartsWith("<body", ToLower())) {
      return false;
    } else if (avail.length() > 1 && TAG_NAME_TABLE.Get(avail[1])) {
      avail.set_begin(avail.FindFirstOf('>').end());
    } else if (!avail.is_empty()) {
      avail = avail.SubString(1);
    }
  }
  return false;
}

bool CharacterEncoding::DetectFromXmlHeader(
    const String &str, StringBuilder *encoding) {
  static const char UCS4_LE[4] = { '\x3C', '\x00', '\x00', '\x00' };
  static const char UCS4_BE[4] = { '\x00', '\x00', '\x00', '\x3C' };
  static const char UCS2_LE[4] = { '\x3C', '\x00', '\x3F', '\x00' };
  static const char UCS2_BE[4] = { '\x00', '\x3C', '\x00', '\x3F' };

  encoding->Clear();
  if (str.StartsWith(UCS4_LE, sizeof(UCS4_LE))) {
    encoding->Assign("UCS-4LE");
  } else if (str.StartsWith(UCS4_BE, sizeof(UCS4_BE))) {
    encoding->Assign("UCS-4BE");
  } else if (str.StartsWith(UCS2_LE, sizeof(UCS2_LE))) {
    encoding->Assign("UCS-2LE");
  } else if (str.StartsWith(UCS2_BE, sizeof(UCS2_BE))) {
    encoding->Assign("UCS-2BE");
  } else if (str.StartsWith("<?xml", ToLower())) {
    String tag(str.begin() + 5, str.Find("?>").begin());
    String charset(tag.Find("encoding", ToLower()).end(), tag.end());
    charset = charset.StripLeft();
    if (charset.StartsWith("=")) {
      charset = charset.SubString(1).StripLeft();
      if (!charset.is_empty()) {
        char quote = charset[0];
        charset = charset.SubString(1);
        charset.set_end(charset.FindFirstOf(quote).begin());
        charset = charset.Strip();
        encoding->Assign(charset, ToUpper());
        return true;
      }
    }
  }
  return false;
}

}  // namespace nwc_toolkit
