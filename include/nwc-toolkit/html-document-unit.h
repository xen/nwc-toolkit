// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_HTML_DOCUMENT_UNIT_H_
#define NWC_TOOLKIT_HTML_DOCUMENT_UNIT_H_

#include "./html-document-attribute.h"

namespace nwc_toolkit {

class HtmlDocumentUnit {
 public:
  enum UnitType {
    UNDEFINED_UNIT = 0,
    TEXT_UNIT = 1,
    TAG_UNIT = 2,
    COMMENT_UNIT = 3,
    OTHER_UNIT = 4
  };

  HtmlDocumentUnit()
      : type_flags_(UNDEFINED_UNIT),
        src_(),
        content_(),
        attributes_(NULL),
        num_attributes_(0) {}
  ~HtmlDocumentUnit() {
    Clear();
  }

  // Common methods.
  UnitType type() const;
  const String &src() const {
    return src_;
  }

  void set_type(UnitType type) {
    type_flags_ = type;
  }
  void set_src(const String &src) {
    src_ = src;
  }

  void Clear() {
    type_flags_ = OTHER_UNIT;
    src_.Clear();
    content_.Clear();
    attributes_ = NULL;
    num_attributes_ = 0;
  }

  // Methods for text unit.
  bool is_cdata_section() const {
    return (type_flags_ & CDATA_SECTION_FLAG) == CDATA_SECTION_FLAG;
  }
  const String &text_content() const {
    return content_;
  }

  void set_cdata_section_flag() {
    type_flags_ |= CDATA_SECTION_FLAG;
  }
  void set_text_content(const String &content) {
    content_ = content;
  }

  // Methods for text unit.
  bool is_start_tag() const {
    return (type_flags_ & START_TAG_FLAG) == START_TAG_FLAG;
  }
  bool is_end_tag() const {
    return (type_flags_ & END_TAG_FLAG) == END_TAG_FLAG;
  }
  bool is_empty_element_tag() const {
    return (type_flags_ & EMPTY_ELEMENT_TAG_FLAG) == EMPTY_ELEMENT_TAG_FLAG;
  }
  const String &tag_name() {
    return content_;
  }
  std::size_t num_attributes() const {
    return num_attributes_;
  };
  const HtmlDocumentAttribute &attribute(std::size_t id) const {
    return attributes_[id];
  }

  void set_start_tag_flag() {
    type_flags_ |= START_TAG_FLAG;
  }
  void set_end_tag_flag() {
    type_flags_ |= END_TAG_FLAG;
  }
  void set_empty_element_tag_flag() {
    type_flags_ |= EMPTY_ELEMENT_TAG_FLAG;
  }
  void set_tag_name(const String &str) {
    content_ = str;
  }
  void set_attributes(const HtmlDocumentAttribute *attributes,
      std::size_t num_attributes) {
    attributes_ = attributes;
    num_attributes_ = num_attributes;
  }

  // Methods for text unit.
  const String &comment() const {
    return content_;
  }

  void set_comment(const String &str) {
    content_ = str;
  }

  // Methods for text unit.
  const String &other_content() const {
    return content_;
  }

  void set_other_content(const String &str) {
    content_ = str;
  }

 private:
  enum {
    UNIT_TYPE_MASK = 0xFF,
    UNIT_FLAGS_MASK = 0xFF00
  };

  enum UnitFlags {
    CDATA_SECTION_FLAG = 1 << 16,
    START_TAG_FLAG = 1 << 17,
    END_TAG_FLAG = 1 << 18,
    EMPTY_ELEMENT_TAG_FLAG = 1 << 19,
  };

  int type_flags_;
  String src_;
  String content_;
  const HtmlDocumentAttribute *attributes_;
  std::size_t num_attributes_;

  // Disallows copy and assignment.
  HtmlDocumentUnit(const HtmlDocumentUnit &);
  HtmlDocumentUnit &operator=(const HtmlDocumentUnit &);
};

inline HtmlDocumentUnit::UnitType HtmlDocumentUnit::type() const {
  switch (type_flags_ & 0x0F) {
    case TEXT_UNIT: {
      return TEXT_UNIT;
    }
    case TAG_UNIT: {
      return TAG_UNIT;
    }
    case COMMENT_UNIT: {
      return COMMENT_UNIT;
    }
    case OTHER_UNIT: {
      return OTHER_UNIT;
    }
    default: {
      return UNDEFINED_UNIT;
    }
  }
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HTML_DOCUMENT_UNIT_H_
