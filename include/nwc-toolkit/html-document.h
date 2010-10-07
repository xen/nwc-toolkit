// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_HTML_DOCUMENT_H_
#define NWC_TOOLKIT_HTML_DOCUMENT_H_

#include <vector>

#include "./html-archive-entry.h"
#include "./html-document-unit.h"
#include "./string-pool.h"

namespace nwc_toolkit {

class HtmlDocument {
 public:
  HtmlDocument();
  ~HtmlDocument() {
    Clear();
  }

  String body() const {
    return body_.str();
  }
  String src_encoding() const {
    return src_encoding_.str();
  }
  String content_type() const {
    return content_type_.str();
  }

  std::size_t num_units() const;
  const HtmlDocumentUnit &unit(std::size_t id) const;

  void Clear();

  bool Parse(const String &body) {
    HtmlArchiveEntry entry;
    entry.set_body(body);
    return Parse(entry);
  }
  bool Parse(const HtmlArchiveEntry &entry);

  void ExtractText(StringBuilder *dest);

 private:
//  enum ParserMode {
//    PLAIN_TEXT_MODE,
//    HTML_MODE,
//    XML_MODE
//  };

  StringBuilder body_;
  StringBuilder src_encoding_;
  StringBuilder content_type_;
  std::vector<HtmlDocumentUnit> units_;

  StringBuilder temp_buf_;
  StringPool string_pool_;
//  HtmlDocumentAttributes attributes_;

  bool ParseAsPlainText(const String &body);
  bool ParseAsXml(const String &body);
  bool ParseAsHtml(const String &body);

  // Disallows copy and assignment.
  HtmlDocument(const HtmlDocument &);
  HtmlDocument &operator=(const HtmlDocument &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HTML_DOCUMENT_H_
