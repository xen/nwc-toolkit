// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CETR_LINE_H_
#define NWC_TOOLKIT_CETR_LINE_H_

#include "./cetr-unit.h"
#include "./string.h"

namespace nwc_toolkit {

class CetrLine {
 public:
  CetrLine()
      : units_(NULL),
        num_units_(0),
        num_tags_(0),
        num_chars_(0),
        num_text_chars_(0) {}
  ~CetrLine() {
    Clear();
  }
  CetrLine(const CetrLine &line)
      : units_(line.units_),
        num_units_(line.num_units_),
        num_tags_(line.num_tags_),
        num_chars_(line.num_chars_),
        num_text_chars_(line.num_text_chars_) {}

  CetrLine &operator=(const CetrLine &line) {
    units_ = line.units_;
    num_units_ = line.num_units_;
    num_tags_ = line.num_tags_;
    num_chars_ = line.num_chars_;
    num_text_chars_ = line.num_text_chars_;
    return *this;
  }

  const CetrUnit &unit(std::size_t id) const {
    return units_[id];
  }
  std::size_t num_units() const {
    return num_units_;
  }
  std::size_t num_tags() const {
    return num_tags_;
  }
  std::size_t num_chars() const {
    return num_chars_;
  }
  std::size_t num_text_chars() const {
    return num_text_chars_;
  }

  void set_units(const CetrUnit *units) {
    units_ = units;
  }
  void set_num_units(std::size_t num_units) {
    num_units_ = num_units;
  }
  void set_num_tags(std::size_t num_tags) {
    num_tags_ = num_tags;
  }
  void set_num_chars(std::size_t num_chars) {
    num_chars_ = num_chars;
  }
  void set_num_text_chars(std::size_t num_text_chars) {
    num_text_chars_ = num_text_chars;
  }

  void Clear() {
    units_ = NULL;
    num_units_ = 0;
    num_tags_ = 0;
    num_chars_ = 0;
    num_text_chars_ = 0;
  }

 private:
  const CetrUnit *units_;
  std::size_t num_units_;
  std::size_t num_tags_;
  std::size_t num_chars_;
  std::size_t num_text_chars_;

  // Copyable.
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CETR_LINE_H_
