// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_HTML_DOCUMENT_H_
#define NWC_TOOLKIT_HTML_DOCUMENT_H_

#include <vector>

#include "./html-archive-entry.h"
#include "./html-unit.h"
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
  const HtmlUnit &unit(std::size_t id) const {
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

  static bool IsBlockTag(const String &tag_name);

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

  enum TextExtractorModeFlags {
    SCRIPT_MODE_FLAG = 1 << 20,
    STYLE_MODE_FLAG = 1 << 21,
    XMP_MODE_FLAG = 1 << 22,
    PLAINTEXT_MODE_FLAG = 1 << 23,
    PRE_MODE_FLAG = 1 << 24,
    LISTING_MODE_FLAG = 1 << 25,
    TEXTAREA_MODE_FLAG = 1 << 26,

    PLAIN_MODE_FLAGS = SCRIPT_MODE_FLAG | STYLE_MODE_FLAG
        | XMP_MODE_FLAG | PLAINTEXT_MODE_FLAG,
    INVISIBLE_MODE_FLAGS = SCRIPT_MODE_FLAG | STYLE_MODE_FLAG,
    PRE_MODE_FLAGS = PRE_MODE_FLAG | LISTING_MODE_FLAG | TEXTAREA_MODE_FLAG
  };

  enum EndOfLineHandler {
    KEEP_END_OF_LINE,
    REPLACE_END_OF_LINE
  };

  StringBuilder body_;
  StringBuilder src_encoding_;
  StringBuilder content_type_;
  ParserMode parser_mode_;
  std::vector<HtmlUnit> units_;
  std::vector<HtmlAttribute> attributes_;
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

  static void AppendEndOfLineToText(StringBuilder *text);
  static void AppendToText(const String &str,
      EndOfLineHandler end_of_line_handler, StringBuilder *text);

  static void UpdateTextExtractorModeFlags(
      const HtmlUnit &unit, int *mode_flags);

  // Disallows copy and assignment.
  HtmlDocument(const HtmlDocument &);
  HtmlDocument &operator=(const HtmlDocument &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HTML_DOCUMENT_H_
