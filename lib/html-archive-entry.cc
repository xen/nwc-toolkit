// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/html-archive-entry.h>

#include <cstdlib>
#include <limits>

#include <nwc-toolkit/character-encoding.h>

namespace nwc_toolkit {

void HtmlArchiveEntry::Clear() {
  url_.Clear();
  status_code_ = 0;
  header_.Clear();
  body_.Clear();
}

bool HtmlArchiveEntry::Read(InputFile *file) {
  String url_line;
  if (!file->ReadLine(&url_line)) {
    return false;
  }
  set_url(url_line.StripRight());

  int status_code = 0;
  if (!ReadInt(file, &status_code)) {
    return false;
  }
  set_status_code(status_code);

  int header_length = 0;
  if (!ReadInt(file, &header_length)) {
    return false;
  }
  String header;
  if (!file->Read(header_length, &header)) {
    return false;
  }
  set_header(header);

  int body_length;
  if (!ReadInt(file, &body_length)) {
    return false;
  }
  String body;
  if (!file->Read(body_length, &body)) {
    return false;
  }
  set_body(body);
  return true;
}

bool HtmlArchiveEntry::Write(OutputFile *file) const {
  if (!file->Write(url()) || !file->Write("\n")) {
    return false;
  }

  if (!WriteInt(status_code(), file)) {
    return false;
  }

  if (!WriteInt(static_cast<int>(header_.length()), file) ||
      !file->Write(header())) {
    return false;
  }

  if (!WriteInt(static_cast<int>(body_.length()), file) ||
      !file->Write(body())) {
    return false;
  }
  return true;
}

bool HtmlArchiveEntry::ExtractUnicodeBody(StringBuilder *unicode_body,
    StringBuilder *src_encoding) const {
  unicode_body->Clear();

  if (CharacterEncoding::DetectFromBOM(body(), src_encoding)) {
    if (CharacterEncoding::Convert(src_encoding->str(), body(),
        "UTF-8", unicode_body)) {
      return true;
    }
  }

  int encoding_flags = 0;

  if (CharacterEncoding::DetectFromResponseHeader(header(), src_encoding)) {
    if (CharacterEncoding::Convert(src_encoding->str(), body(),
        "UTF-8", unicode_body)) {
      return true;
    }
    encoding_flags |= DetectEncodingFlags(src_encoding->str());
  }

  if (CharacterEncoding::DetectFromXmlHeader(body(), src_encoding)) {
    if (CharacterEncoding::Convert(src_encoding->str(), body(),
        "UTF-8", unicode_body)) {
      return true;
    }
    encoding_flags |= DetectEncodingFlags(src_encoding->str());
  }

  if (CharacterEncoding::DetectFromHtmlHeader(body(), src_encoding)) {
    if (CharacterEncoding::Convert(src_encoding->str(), body(),
        "UTF-8", unicode_body)) {
      return true;
    }
    encoding_flags |= DetectEncodingFlags(src_encoding->str());
  }

  if (TestEncodings(encoding_flags, unicode_body, src_encoding) ||
      TestEncodings(~encoding_flags, unicode_body, src_encoding)) {
    return true;
  }

  src_encoding->Clear();
  return false;
}

bool HtmlArchiveEntry::ExtractContentType(StringBuilder *content_type) const {
  static const String CONTENT_TYPE_FIELD_NAME = "Content-Type:";

  content_type->Clear();
  for (String avail = header(); !avail.is_empty();
      avail.set_begin(avail.FindFirstOf('\n').end())) {
    if (!avail.StartsWith(CONTENT_TYPE_FIELD_NAME, ToLower())) {
      continue;
    }
    String field_value = avail.SubString(CONTENT_TYPE_FIELD_NAME.length());
    field_value.set_end(field_value.FindFirstOf(";\r\n").begin());
    field_value = field_value.Strip();
    if (!field_value.is_empty()) {
      content_type->Assign(field_value, ToLower());
      return true;
    }
  }
  return false;
}

bool HtmlArchiveEntry::ReadInt(InputFile *file, int *value) {
  String line;
  if (!file->ReadLine(&line)) {
    return false;
  }
  char *value_end = NULL;
  long long_value = std::strtol(line.ptr(), &value_end, 10);
  if (*value_end != '\n') {
    return false;
  }
  if ((long_value < 0) || (long_value > std::numeric_limits<int>::max())) {
    return false;
  }
  *value = static_cast<int>(long_value);
  return true;
}

bool HtmlArchiveEntry::WriteInt(int value, OutputFile *file) {
  if (value < 0) {
    return false;
  }
  char buf[16];
  std::size_t pos = sizeof(buf);
  buf[--pos] = '\n';
  do {
    buf[--pos] = '0' + (value % 10);
    value /= 10;
  } while (value > 0);
  return file->Write(String(buf + pos, sizeof(buf) - pos));
}

int HtmlArchiveEntry::DetectEncodingFlags(const String &encoding) {
  int encoding_flags = 0;
  if (!encoding.Find("SJIS", ToUpper()).is_empty() ||
      !encoding.Find("SJIS", ToUpper()).is_empty()) {
    encoding_flags |= SHIFT_JIS_FLAG;
  }

  if (!encoding.Find("EUC", ToUpper()).is_empty()) {
    encoding_flags |= EUC_JP_FLAG;
  }

  if (!encoding.Find("2022", ToUpper()).is_empty()) {
    encoding_flags |= ISO_2022_JP_FLAG;
  }

  if (!encoding.Find("UTF", ToUpper()).is_empty() ||
      !encoding.Find("UNICODE", ToUpper()).is_empty()) {
    encoding_flags |= UTF_8_FLAG;
  }
  return encoding_flags;
}

bool HtmlArchiveEntry::TestEncodings(int encoding_flags,
    StringBuilder *unicode_body, StringBuilder *src_encoding) const {
  if (((encoding_flags & SHIFT_JIS_FLAG) == SHIFT_JIS_FLAG)) {
    if (CharacterEncoding::Convert("CP932", body(), "UTF-8", unicode_body)) {
      src_encoding->Assign("CP932");
      return true;
    }
  }

  if ((encoding_flags & EUC_JP_FLAG) == EUC_JP_FLAG) {
    if (CharacterEncoding::Convert("EUC-JP-MS", body(),
        "UTF-8", unicode_body)) {
      src_encoding->Assign("EUC-JP-MS");
      return true;
    }
  }

  if ((encoding_flags & ISO_2022_JP_FLAG) == ISO_2022_JP_FLAG) {
    if (CharacterEncoding::Convert("ISO-2022-JP-MS", body(),
        "UTF-8", unicode_body)) {
      src_encoding->Assign("ISO-2022-JP-MS");
      return true;
    } else if (CharacterEncoding::Convert("ISO-2022-JP-2", body(),
        "UTF-8", unicode_body)) {
      src_encoding->Assign("ISO-2022-JP-2");
      return true;
    }
  }

  if ((encoding_flags & UTF_8_FLAG) == UTF_8_FLAG) {
    if (CharacterEncoding::Convert("UTF-8", body(), "UTF-8", unicode_body)) {
      src_encoding->Assign("UTF-8");
      return true;
    }
  }
  return false;
}

}  // namespace nwc_toolkit
