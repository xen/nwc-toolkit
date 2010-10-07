// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_HTML_DOCUMENT_ATTRIBUTE_H_
#define NWC_TOOLKIT_HTML_DOCUMENT_ATTRIBUTE_H_

#include "./string.h"

namespace nwc_toolkit {

class HtmlDocumentAttribute {
 public:
  HtmlDocumentAttribute() : name_(), value_() {}
  ~HtmlDocumentAttribute() {}

  const String &name() const {
    return name_;
  }
  const String &value() const {
    return value_;
  }

  void set_name(const String &name) {
    name_ = name;
  }
  void set_value(const String &value) {
    value_ = value;
  }

  void Clear() {
    name_.Clear();
    value_.Clear();
  }

 private:
  String name_;
  String value_;

  // Disallows copy and assignment.
  HtmlDocumentAttribute(const HtmlDocumentAttribute &);
  HtmlDocumentAttribute &operator=(const HtmlDocumentAttribute &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HTML_DOCUMENT_ATTRIBUTE_H_
