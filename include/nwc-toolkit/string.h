// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_STRING_H_
#define NWC_TOOLKIT_STRING_H_

#include "./char-cond.h"
#include "./char-filter.h"
#include "./char-table.h"
#include "./char-type.h"
#include "./int-traits.h"

namespace nwc_toolkit {

class String {
 public:
  String() : ptr_(NULL), length_(0) {}
  String(const char *str) : ptr_(str), length_(0) {
    if (str != NULL) {
      while (str[length_] != '\0') {
        ++length_;
      }
    }
  }
  String(const char *ptr, std::size_t length) : ptr_(ptr), length_(length) {}
  String(const char *begin, const char *end)
      : ptr_(begin), length_(end - begin) {}
  String(const String &str) : ptr_(str.ptr_), length_(str.length_) {}
  ~String() {}

  const char &operator[](std::size_t index) const {
    return ptr_[index];
  }

  const char *ptr() const {
    return ptr_;
  }
  std::size_t length() const {
    return length_;
  }

  void set_ptr(const char *ptr) {
    ptr_ = ptr;
  }
  void set_length(std::size_t length) {
    length_ = length;
  }

  const char *begin() const {
    return ptr_;
  }
  const char *end() const {
    return ptr_ + length_;
  }

  String &set_begin(const char *begin) {
    length_ -= begin - ptr_;
    ptr_ = begin;
    return *this;
  }
  String &set_end(const char *end) {
    length_ = end - ptr_;
    return *this;
  }

  bool is_empty() const {
    return length_ == 0;
  }

  void Clear() {
    length_ = 0;
  }

  String &operator=(const char *str) {
    return Assign(str);
  }
  String &operator=(const String &str) {
    return Assign(str);
  }

  String &Assign(const char *str) {
    return Assign(String(str));
  }
  String &Assign(const char *ptr, std::size_t length) {
    return Assign(String(ptr, length));
  }
  String &Assign(const char *begin, const char *end) {
    return Assign(String(begin, end));
  }
  String &Assign(const String &str) {
    ptr_ = str.ptr_;
    length_ = str.length_;
    return *this;
  }

  String SubString(std::size_t pos) const {
    return String(ptr_ + pos, length_ - pos);
  }
  String SubString(std::size_t pos, std::size_t length) const {
    return String(ptr_ + pos, length);
  }

  bool operator==(const char *str) const {
    for (std::size_t i = 0; i < length_; ++i) {
      if (str[i] == '\0' || ptr_[i] != str[i]) {
        return false;
      }
    }
    return str[length_] == '\0';
  }
  bool operator==(const String &str) const {
    if (length_ != str.length_) {
      return false;
    }
    for (std::size_t i = 0; i < length_; ++i) {
      if (ptr_[i] != str[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const char *str) const {
    return !(*this == str);
  }
  bool operator!=(const String &str) const {
    return !(*this == str);
  }

  bool operator<(const char *str) const {
    return Compare(str) < 0;
  }
  bool operator<(const String &str) const {
    return Compare(str) < 0;
  }
  bool operator<=(const char *str) const {
    return Compare(str) <= 0;
  }
  bool operator<=(const String &str) const {
    return Compare(str) <= 0;
  }
  bool operator>(const char *str) const {
    return Compare(str) > 0;
  }
  bool operator>(const String &str) const {
    return Compare(str) > 0;
  }
  bool operator>=(const char *str) const {
    return Compare(str) >= 0;
  }
  bool operator>=(const String &str) const {
    return Compare(str) >= 0;
  }

  bool Contains(char c) const {
    return Contains(CondChar(c));
  }
  bool Contains(const char *str) const {
    return Contains(CondCString(str));
  }
  bool Contains(const char *ptr, std::size_t length) const {
    return Contains(CondString(ptr, length));
  }
  bool Contains(const String &str) const {
    return Contains(str.ptr(), str.length());
  }
  bool Contains(const CharTable &table) const {
    return Contains(CondTable(table));
  }
  template <typename T>
  bool Contains(T cond) const {
    for (std::size_t i = 0; i < length_; ++i) {
      if (cond(ptr_[i])) {
        return true;
      }
    }
    return false;
  }

  int Compare(const char *str) const {
    return Compare(str, KeepAsIs());
  }
  int Compare(const char *ptr, std::size_t length) const {
    return Compare(ptr, length, KeepAsIs());
  }
  int Compare(const String &str) const {
    return Compare(str, KeepAsIs());
  }
  template <typename T>
  int Compare(const char *str, T filter) const {
    return Compare(str, filter, typename IntTraits<T>::Tag());
  }
  template <typename T>
  int Compare(const char *str, T filter, IsInt) const {
    return Compare(str, static_cast<std::size_t>(filter));
  }
  template <typename T>
  int Compare(const char *str, T filter, IsNotInt) const {
    for (std::size_t i = 0; i < length_; ++i) {
      if (str[i] == '\0') {
        return 1;
      }
      unsigned char lc = static_cast<unsigned char>(filter(ptr_[i]));
      unsigned char rc = static_cast<unsigned char>(filter(str[i]));
      if (lc != rc) {
        return lc - rc;
      }
    }
    return (str[length_] == '\0') ? 0 : -1;
  }
  template <typename T>
  int Compare(const char *str, std::size_t length, T filter) const {
    return Compare(String(str, length), filter);
  }
  template <typename T>
  int Compare(const String &str, T filter) const {
    for (std::size_t i = 0; i < length_; ++i) {
      if (i == str.length()) {
        return 1;
      }
      unsigned char lc = static_cast<unsigned char>(filter(ptr_[i]));
      unsigned char rc = static_cast<unsigned char>(filter(str[i]));
      if (lc != rc) {
        return lc - rc;
      }
    }
    return (length_ == str.length()) ? 0 : -1;
  }

  bool StartsWith(const char *str) const {
    return StartsWith(str, KeepAsIs());
  }
  bool StartsWith(const char *ptr, std::size_t length) const {
    return StartsWith(ptr, length, KeepAsIs());
  }
  bool StartsWith(const String &str) const {
    return StartsWith(str, KeepAsIs());
  }
  template <typename T>
  bool StartsWith(const char *str, T filter) const {
    return StartsWith(str, filter, typename IntTraits<T>::Tag());
  }
  template <typename T>
  bool StartsWith(const char *str, T filter, IsInt) const {
    return StartsWith(str, static_cast<std::size_t>(filter));
  }
  template <typename T>
  bool StartsWith(const char *str, T filter, IsNotInt) const {
    for (std::size_t i = 0; i < length_; ++i) {
      if (str[i] == '\0') {
        return true;
      } else if (filter(ptr_[i]) != filter(str[i])) {
        return false;
      }
    }
    return str[length_] == '\0';
  }
  template <typename T>
  bool StartsWith(const char *ptr, std::size_t length, T filter) const {
    return StartsWith(String(ptr, length), filter);
  }
  template <typename T>
  bool StartsWith(const String &str, T filter) const {
    if (length_ < str.length()) {
      return false;
    }
    for (std::size_t i = 0; i < str.length(); ++i) {
      if (filter(ptr_[i]) != filter(str[i])) {
        return false;
      }
    }
    return true;
  }

  bool EndsWith(const char *str) const {
    return EndsWith(str, KeepAsIs());
  }
  bool EndsWith(const char *ptr, std::size_t length) const {
    return EndsWith(ptr, length, KeepAsIs());
  }
  bool EndsWith(const String &str) const {
    return EndsWith(str, KeepAsIs());
  }
  template <typename T>
  bool EndsWith(const char *str, T filter) const {
    return EndsWith(str, filter, typename IntTraits<T>::Tag());
  }
  template <typename T>
  bool EndsWith(const char *str, T filter, IsInt) const {
    return EndsWith(str, static_cast<std::size_t>(filter));
  }
  template <typename T>
  bool EndsWith(const char *str, T filter, IsNotInt) const {
    std::size_t length = 0;
    for ( ; str[length] != '\0'; ++length) {
      if (length > length_) {
        return false;
      }
    }
    std::size_t offset = length_ - length;
    for (std::size_t i = 0; i < length; ++i) {
      if (filter(ptr_[offset + i]) != filter(str[i]))
        return false;
    }
    return true;
  }
  template <typename T>
  bool EndsWith(const char *ptr, std::size_t length, T filter) const {
    return EndsWith(String(ptr, length), filter);
  }
  template <typename T>
  bool EndsWith(const String &str, T filter) const {
    if (length_ < str.length()) {
      return false;
    }
    std::size_t offset = length_ - str.length();
    for (std::size_t i = 0; i < str.length(); ++i) {
      if (filter(ptr_[offset + i]) != filter(str[i])) {
        return false;
      }
    }
    return true;
  }

  String Find(const char *str) const {
    return Find(str, KeepAsIs());
  }
  String Find(const char *ptr, std::size_t length) const {
    return Find(ptr, length, KeepAsIs());
  }
  String Find(const String &str) const {
    return Find(str, KeepAsIs());
  }
  template <typename T>
  String Find(const char *str, T filter) const {
    return Find(str, filter, typename IntTraits<T>::Tag());
  }
  template <typename T>
  String Find(const char *str, T filter, IsInt) const {
    return Find(str, static_cast<std::size_t>(filter));
  }
  template <typename T>
  String Find(const char *str, T filter, IsNotInt) const {
    for (std::size_t i = 0; i < length_; ++i) {
      const char *p = str;
      for (std::size_t j = i; j < length_ && *p != '\0'; ++j, ++p) {
        if (filter(*p) != filter(ptr_[j])) {
          break;
        }
      }
      if (*p == '\0') {
        return SubString(i, p - str);
      }
    }
    return SubString(length_);
  }
  template <typename T>
  String Find(const char *ptr, std::size_t length, T filter) const {
    return Find(String(ptr, length), filter);
  }
  template <typename T>
  String Find(const String &str, T filter) const {
    if (length_ < str.length()) {
      return SubString(length_);
    }
    std::size_t max_pos = length_ - str.length() + 1;
    for (std::size_t i = 0; i < max_pos; ++i) {
      std::size_t k = 0;
      for (std::size_t j = i; j < length_ && k < str.length(); ++j, ++k) {
        if (filter(ptr_[j]) != filter(str[k])) {
          break;
        }
      }
      if (k == str.length()) {
        return SubString(i, str.length());
      }
    }
    return SubString(length_);
  }

  String FindFirstOf(char c) const {
    return FindFirstOf(CondChar(c));
  }
  String FindFirstOf(const char *str) const {
    return FindFirstOf(CondCString(str));
  }
  String FindFirstOf(const char *ptr, std::size_t length) const {
    return FindFirstOf(CondString(ptr, length));
  }
  String FindFirstOf(const String &str) const {
    return FindFirstOf(str.ptr(), str.length());
  }
  String FindFirstOf(const CharTable &table) const {
    return FindFirstOf(CondTable(table));
  }
  template <typename T>
  String FindFirstOf(T cond) const {
    for (std::size_t i = 0; i < length_; ++i) {
      if (cond(ptr_[i])) {
        return SubString(i, 1);
      }
    }
    return SubString(length_);
  }

  String FindFirstNotOf(char c) const {
    return FindFirstNotOf(CondChar(c));
  }
  String FindFirstNotOf(const char *str) const {
    return FindFirstNotOf(CondCString(str));
  }
  String FindFirstNotOf(const char *ptr, std::size_t length) const {
    return FindFirstNotOf(CondString(ptr, length));
  }
  String FindFirstNotOf(const String &str) const {
    return FindFirstNotOf(str.ptr(), str.length());
  }
  String FindFirstNotOf(const CharTable &table) const {
    return FindFirstNotOf(CondTable(table));
  }
  template <typename T>
  String FindFirstNotOf(T cond) const {
    for (std::size_t i = 0; i < length_; ++i) {
      if (!cond(ptr_[i])) {
        return SubString(i, 1);
      }
    }
    return SubString(length_);
  }

  String FindLastOf(char c) const {
    return FindLastOf(CondChar(c));
  }
  String FindLastOf(const char *str) const {
    return FindLastOf(CondCString(str));
  }
  String FindLastOf(const char *ptr, std::size_t length) const {
    return FindLastOf(CondString(ptr, length));
  }
  String FindLastOf(const String &str) const {
    return FindLastOf(str.ptr(), str.length());
  }
  String FindLastOf(const CharTable &table) const {
    return FindLastOf(CondTable(table));
  }
  template <typename T>
  String FindLastOf(T cond) const {
    for (std::size_t i = 1; i <= length_; ++i) {
      if (cond(ptr_[length_ - i])) {
        return SubString(length_ - i, 1);
      }
    }
    return SubString(length_);
  }

  String FindLastNotOf(char c) const {
    return FindLastNotOf(CondChar(c));
  }
  String FindLastNotOf(const char *str) const {
    return FindLastNotOf(CondCString(str));
  }
  String FindLastNotOf(const char *ptr, std::size_t length) const {
    return FindLastNotOf(CondString(ptr, length));
  }
  String FindLastNotOf(const String &str) const {
    return FindLastNotOf(str.ptr(), str.length());
  }
  String FindLastNotOf(const CharTable &table) const {
    return FindLastNotOf(CondTable(table));
  }
  template <typename T>
  String FindLastNotOf(T cond) const {
    for (std::size_t i = 1; i <= length_; ++i) {
      if (!cond(ptr_[length_ - i])) {
        return SubString(length_ - i, 1);
      }
    }
    return SubString(length_);
  }

  String Strip() const {
    return Strip(IsSpace());
  }
  String Strip(char c) const {
    return Strip(CondChar(c));
  }
  String Strip(const char *str) const {
    return Strip(CondCString(str));
  }
  String Strip(const char *ptr, std::size_t length) const {
    return Strip(CondString(ptr, length));
  }
  String Strip(const String &str) const {
    return Strip(str.ptr(), str.length());
  }
  String Strip(const CharTable &table) const {
    return Strip(CondTable(table));
  }
  template <typename T>
  String Strip(T cond) const {
    return StripLeft(cond).StripRight(cond);
  }

  String StripLeft() const {
    return StripLeft(IsSpace());
  }
  String StripLeft(char c) const {
    return StripLeft(CondChar(c));
  }
  String StripLeft(const char *str) const {
    return StripLeft(CondCString(str));
  }
  String StripLeft(const char *ptr, std::size_t length) const {
    return StripLeft(CondString(ptr, length));
  }
  String StripLeft(const String &str) const {
    return StripLeft(str.ptr(), str.length());
  }
  String StripLeft(const CharTable &table) const {
    return StripLeft(CondTable(table));
  }
  template <typename T>
  String StripLeft(T cond) const {
    std::size_t index = 0;
    while (index < length_ && cond(ptr_[index])) {
      ++index;
    }
    return SubString(index, length_ - index);
  }

  String StripRight() const {
    return StripRight(IsSpace());
  }
  String StripRight(char c) const {
    return StripRight(CondChar(c));
  }
  String StripRight(const char *str) const {
    return StripRight(CondCString(str));
  }
  String StripRight(const char *ptr, std::size_t length) const {
    return StripRight(CondString(ptr, length));
  }
  String StripRight(const String &str) const {
    return StripRight(str.ptr(), str.length());
  }
  String StripRight(const CharTable &table) const {
    return StripRight(CondTable(table));
  }
  template <typename T>
  String StripRight(T cond) const {
    std::size_t index = length_;
    while (index > 0 && cond(ptr_[index - 1])) {
      --index;
    }
    return SubString(0, index);
  }

 private:
  const char *ptr_;
  std::size_t length_;

  static std::size_t LengthOf(const char *str) {
    std::size_t length = 0;
    while (str[length] != '\0') {
      ++length;
    }
    return length;
  }

  // Copyable.
};

template <typename T>
T &operator<<(T &stream, const String &str) {
  stream.write(str.ptr(), str.length());
  return stream;
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_STRING_H_
