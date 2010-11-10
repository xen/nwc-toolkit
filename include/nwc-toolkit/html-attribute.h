// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_HTML_ATTRIBUTE_H_
#define NWC_TOOLKIT_HTML_ATTRIBUTE_H_

#include "./string.h"

namespace nwc_toolkit {

class HtmlAttribute {
 public:
  HtmlAttribute() : name_(), value_() {}

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

  // Copyable.
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HTML_ATTRIBUTE_H_
