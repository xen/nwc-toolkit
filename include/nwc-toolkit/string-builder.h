// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_STRING_BUILDER_H_
#define NWC_TOOLKIT_STRING_BUILDER_H_

#include "./string.h"

namespace nwc_toolkit {

class StringBuilder {
 public:
  StringBuilder() : buf_(InitialBuf()), length_(0), size_(0) {}
  ~StringBuilder() {
    if (buf_ != InitialBuf()) {
      delete [] buf_;
    }
  }

  char &operator[](std::size_t index) {
    return buf_[index];
  }
  const char &operator[](std::size_t index) const {
    return buf_[index];
  }

  char *buf() {
    return buf_;
  }
  const char *buf() const {
    return buf_;
  }

  std::size_t length() const {
    return length_;
  }
  std::size_t size() const {
    return size_;
  }

  const char *ptr() const {
    return buf_;
  }
  String str() const {
    return String(buf_, length_);
  }

  bool is_empty() const {
    return length_ == 0;
  }

  void Clear() {
    length_ = 0;
  }

  StringBuilder &operator=(const String &str) {
    return Assign(str);
  }

  StringBuilder &Assign(const char *ptr, std::size_t length) {
    return Assign(ptr, length, KeepAsIs());
  }
  StringBuilder &Assign(const String &str) {
    return Assign(str, KeepAsIs());
  }
  template <typename T>
  StringBuilder &Assign(const char *str, T filter) {
    return Assign(str, filter, typename IntTraits<T>::Tag());
  }
  template <typename T>
  StringBuilder &Assign(const char *str, T filter, IsInt) {
    return Assign(str, static_cast<std::size_t>(filter));
  }
  template <typename T>
  StringBuilder &Assign(const char *str, T filter, IsNotInt) {
    return Assign(String(str), filter);
  }
  template <typename T>
  StringBuilder &Assign(const char *ptr, std::size_t length, T filter)
  { return Assign(String(ptr, length), filter); }
  template <typename T>
  StringBuilder &Assign(const String &str, T filter) {
    Clear();
    Append(str, filter);

    // gcc-4.4.1 generates wrong binary if this meaningless assignment
    // does not exist. Hmm...
    length_ = str.length();
    return *this;
  }

  StringBuilder &Append() {
    if (length_ >= size_) {
      ResizeBuf(length_ + 1);
    }
    buf_[length_] = '\0';
    return *this;
  }

  StringBuilder &Append(char byte) {
    return Append(byte, KeepAsIs());
  }
  template <typename T>
  StringBuilder &Append(char byte, T filter) {
    if (length_ >= size_) {
      ResizeBuf(length_ + 1);
    }
    buf_[length_++] = filter(byte);
    return *this;
  }

  StringBuilder &Append(const char *ptr, std::size_t length) {
    return Append(ptr, length, KeepAsIs());
  }
  StringBuilder &Append(const String &str) {
    return Append(str, KeepAsIs());
  }
  template <typename T>
  StringBuilder &Append(const char *str, T filter) {
    return Append(str, filter, typename IntTraits<T>::Tag());
  }
  template <typename T>
  StringBuilder &Append(const char *str, T filter, IsInt) {
    return Append(str, static_cast<std::size_t>(filter));
  }
  template <typename T>
  StringBuilder &Append(const char *str, T filter, IsNotInt) {
    return Append(String(str), filter);
  }
  template <typename T>
  StringBuilder &Append(const char *ptr, std::size_t length, T filter)
  { return Append(String(ptr, length), filter); }
  template <typename T>
  StringBuilder &Append(const String &str, T filter) {
    if (length_ + str.length() > size_) {
      ResizeBuf(length_ + str.length());
    }
    for (std::size_t i = 0; i < str.length(); ++i) {
      buf_[length_++] = filter(str[i]);
    }
    return *this;
  }

  void Resize(std::size_t length) {
    if (length > size_) {
      ResizeBuf(length);
    }
    length_ = length;
  }
  void Reserve(std::size_t size) {
    if (size > size_) {
      ResizeBuf(size);
    }
  }

 private:
  char *buf_;
  std::size_t length_;
  std::size_t size_;

  static char *InitialBuf() {
    static char initial_buf = '\0';
    return &initial_buf;
  }

  void ResizeBuf(std::size_t size);

  // Disallows copy and assignment.
  StringBuilder(const StringBuilder &);
  StringBuilder &operator=(const StringBuilder &);
};

inline void StringBuilder::ResizeBuf(std::size_t size) {
  std::size_t new_buf_size = (size_ != 0) ? size_ : sizeof(buf_);
  while (new_buf_size < size) {
    new_buf_size *= 2;
  }
  char *new_buf = new char[new_buf_size];
  for (std::size_t i = 0; i < length_; ++i) {
    new_buf[i] = buf_[i];
  }
  if (buf_ != InitialBuf()) {
    delete [] buf_;
  }
  buf_ = new_buf;
  size_ = new_buf_size;
}

template <typename T>
T &operator<<(T &stream, const StringBuilder &builder) {
  stream << builder.str();
  return stream;
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_STRING_BUILDER_H_
