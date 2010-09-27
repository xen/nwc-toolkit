// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CHAR_COND_H_
#define NWC_TOOLKIT_CHAR_COND_H_

#include "./char-table.h"

namespace nwc_toolkit {

class CondChar {
 public:
  explicit CondChar(unsigned char c) : c_(c) {}
  CondChar(const CondChar &cond) : c_(cond.c_) {}
  ~CondChar() {}

  bool operator()(unsigned char c) const {
    return c == c_;
  }

 private:
  unsigned char c_;

  // Assignment is not allowed.
  CondChar &operator=(const CondChar &);
};

class CondCString {
 public:
  explicit CondCString(const char *str) : str_(str) {}
  CondCString(const CondCString &cond) : str_(cond.str_) {}
  ~CondCString() {}

  bool operator()(unsigned char c) const {
    for (const char *p = str_; *p != '\0'; ++p) {
      if (c == static_cast<unsigned char>(*p))
        return true;
    }
    return false;
  }

 private:
  const char *str_;

  // Assignment is not allowed.
  CondCString &operator=(const CondCString &);
};

class CondString {
 public:
  CondString(const char *str, std::size_t length)
      : str_(str), length_(length) {}
  CondString(const CondString &cond)
      : str_(cond.str_), length_(cond.length_) {}
  ~CondString() {}

  bool operator()(unsigned char c) const {
    for (std::size_t i = 0; i < length_; ++i) {
      if (c == static_cast<unsigned char>(str_[i]))
        return true;
    }
    return false;
  }

 private:
  const char *str_;
  std::size_t length_;

  // Assignment is not allowed.
  CondString &operator=(const CondString &);
};

class CondTable {
 public:
  explicit CondTable(const CharTable &table) : table_(table) {}
  CondTable(const CondTable &cond) : table_(cond.table_) {}
  ~CondTable() {}

  bool operator()(unsigned char c) const {
    return table_.Get(c);
  }

 private:
  const CharTable &table_;

  // Assignment is not allowed.
  CondString &operator=(const CondString &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CHAR_COND_H_
