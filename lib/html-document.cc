// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/html-document.h>

#include <nwc-toolkit/character-encoding.h>

namespace nwc_toolkit {

HtmlDocument::HtmlDocument()
    : body_(),
      src_encoding_(),
      content_type_(),
      units_(),
      temp_buf_(),
      string_pool_() {}

void HtmlDocument::Clear() {
  body_.Clear();
  src_encoding_.Clear();
  content_type_.Clear();
  units_.clear();
  temp_buf_.Clear();
  string_pool_.Clear();
}

bool HtmlDocument::Parse(const HtmlArchiveEntry &entry) {
  Clear();

  if (!entry.ExtractUnicodeBody(&body_, &src_encoding_)) {
    return false;
  }

  static const String UTF_8_BOM = "\xEF\xBB\xBF";

  String body = body_.str();
  while (body.StartsWith(UTF_8_BOM)) {
    body = body.SubString(UTF_8_BOM.length());
  }

  String path(entry.url().begin(), entry.url().FindFirstOf("?#").begin());
  entry.ExtractContentType(&content_type_);

  if ((content_type_.str() == "text/plain") || path.EndsWith(".txt")) {
    if (ParseAsPlainText(body)) {
      return true;
    }
  } else if (ParseAsXml(body) || ParseAsHtml(body)) {
    return true;
  }
  return false;
}

void HtmlDocument::ExtractText(StringBuilder *dest) {
}

bool HtmlDocument::ParseAsPlainText(const String &body) {
  return true;
}

bool HtmlDocument::ParseAsXml(const String &body) {
  return true;
}

bool HtmlDocument::ParseAsHtml(const String &body) {
  return true;
}

}  // namespace nwc_toolkit
