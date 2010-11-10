// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CETR_UNIT_H_
#define NWC_TOOLKIT_CETR_UNIT_H_

#include "./html-unit.h"

namespace nwc_toolkit {

class CetrUnit {
 public:
  enum UnitType {
    TAG_UNIT,
    TEXT_UNIT,
    INVISIBLE_UNIT
  };

  CetrUnit() : type_(INVISIBLE_UNIT), unit_(NULL), content_() {}
  ~CetrUnit() {
    Clear();
  }
  CetrUnit(const CetrUnit &unit)
      : type_(unit.type_),
        unit_(unit.unit_),
        content_(unit.content_) {}

  CetrUnit &operator=(const CetrUnit &unit) {
    type_ = unit.type_;
    unit_ = unit.unit_;
    content_ = unit.content_;
    return *this;
  }

  UnitType type() const {
    return type_;
  }
  const HtmlUnit &unit() const {
    return *unit_;
  }
  const String &content() const {
    return content_;
  }

  void set_type(UnitType type) {
    type_ = type;
  }
  void set_unit(const HtmlUnit &unit) {
    unit_ = &unit;
  }
  void set_content(const String &content) {
    content_ = content;
  }

  void Clear() {
    type_ = INVISIBLE_UNIT;
    unit_ = NULL;
    content_.Clear();
  }

 private:
  UnitType type_;
  const HtmlUnit *unit_;
  String content_;

  // Copyable.
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CETR_UNIT_H_
