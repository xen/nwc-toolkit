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
  enum ParserMode {
    UNDEFINED_MODE,
    PLAIN_TEXT_MODE,
    HTML_MODE,
    XML_MODE
  };

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
  ParserMode parser_mode() const {
    return parser_mode_;
  }

  std::size_t num_units() const {
    return units_.size();
  }
  const HtmlDocumentUnit &unit(std::size_t id) const {
    return units_[id];
  }

  void Clear();

  bool Parse(const String &body) {
    HtmlArchiveEntry entry;
    entry.set_body(body);
    return Parse(entry);
  }
  bool Parse(const HtmlArchiveEntry &entry);

  void ExtractText(StringBuilder *dest);

 private:
  enum TextUnitFlags {
    CDATA_SECTION_FLAG = 1 << 0,
    PLAIN_TEXT_FLAG = 1 << 1,
  };

  enum TagUnitFlags {
    START_TAG_FLAG = 1 << 10,
    END_TAG_FLAG = 1 << 11,
    EMPTY_ELEMENT_TAG_FLAG = 1 << 12
  };

  StringBuilder body_;
  StringBuilder src_encoding_;
  StringBuilder content_type_;
  ParserMode parser_mode_;
  std::vector<HtmlDocumentUnit> units_;
  std::vector<HtmlDocumentAttribute> attributes_;
  StringPool string_pool_;
  StringBuilder temp_buf_;
  std::vector<char> symbol_stack_;

  void ClearUnits();

  bool ParseAsPlainText(const String &body);
  bool ParseAsXml(const String &body);
  bool ParseAsHtml(const String &body);

  bool ParseXmlTextUnit(String *tag);
  bool ParseXmlTagUnit(String *tag);
  bool ParseXmlCommentUnit(String *tag);
  bool ParseXmlOtherUnit(String *tag);

  void ParseHtmlTagUnit(String *tag);
  void ParseHtmlCommentUnit(String *tag);
  void ParseHtmlOtherUnit(String *tag);
  void ParseHtmlSpecialTag(const String &body_left,
      const String &tag_name, String *tag);

  void AppendTextUnit(const String &src, const String &text_content,
      int text_unit_flags = 0);
  void AppendTagUnit(const String &src, const String &tag_name,
      std::size_t num_attributes, int tag_unit_flags = 0);
  void AppendCommentUnit(const String &src, const String &comment);
  void AppendOtherUnit(const String &src, const String &other_content);

  void FixTagUnits();
  void FixAttributes();

  // Disallows copy and assignment.
  HtmlDocument(const HtmlDocument &);
  HtmlDocument &operator=(const HtmlDocument &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HTML_DOCUMENT_H_
