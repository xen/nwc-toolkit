// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_STRING_POOL_H_
#define NWC_TOOLKIT_STRING_POOL_H_

#include "./string.h"

namespace nwc_toolkit {

class StringPool {
 public:
  class WithEos {};

  explicit StringPool(std::size_t chunk_size = 0);
  ~StringPool();

  std::size_t chunk_size() const {
    return chunk_size_;
  }
  std::size_t num_strings() const {
    return num_strings_;
  }
  std::size_t total_length() const {
    return total_length_;
  }
  std::size_t total_size() const {
    return total_size_;
  }

  bool is_empty() const {
    return num_strings_ == 0;
  }

  void Clear();

  String Append(const char *str) {
    return Append(String(str));
  }
  String Append(const char *ptr, std::size_t length) {
    return Append(String(ptr, length));
  }
  String Append(const String &str);

  String Append(const char *str, WithEos) {
    return Append(String(str), WithEos());
  }
  String Append(const char *ptr, std::size_t length, WithEos) {
    return Append(String(ptr, length), WithEos());
  }
  String Append(const String &str, WithEos);

 private:
  enum { DEFAULT_CHUNK_SIZE = 4096 };

  std::size_t chunk_size_;
  std::size_t num_strings_;
  std::size_t total_length_;
  std::size_t total_size_;

  char **chunks_;
  std::size_t num_chunks_;
  std::size_t max_num_chunks_;

  std::size_t chunk_id_;
  char *ptr_;
  std::size_t avail_;

  char **custom_chunks_;
  std::size_t num_custom_chunks_;
  std::size_t max_num_custom_chunks_;

  String AppendCustomString(const String &str);
  void AppendChunk();

  // Disallows copy and assignment.
  StringPool(const StringPool &);
  StringPool &operator=(const StringPool &);
};

inline StringPool::StringPool(std::size_t chunk_size)
    : chunk_size_((chunk_size == 0) ? DEFAULT_CHUNK_SIZE : chunk_size),
      num_strings_(0),
      total_length_(0),
      total_size_(0),
      chunks_(NULL),
      num_chunks_(0),
      max_num_chunks_(0),
      chunk_id_(0),
      ptr_(NULL),
      avail_(0),
      custom_chunks_(NULL),
      num_custom_chunks_(0),
      max_num_custom_chunks_(0) {}

inline StringPool::~StringPool() {
  Clear();

  for (std::size_t i = 0; i < num_chunks_; ++i) {
    if (chunks_[i] != NULL) {
      delete [] chunks_[i];
      chunks_[i] = NULL;
    }
  }

  if (custom_chunks_ != NULL) {
    delete [] custom_chunks_;
    custom_chunks_ = NULL;
  }
}

inline void StringPool::Clear() {
  num_strings_ = 0;
  total_length_ = 0;
  total_size_ = num_chunks_ * chunk_size_;

  chunk_id_ = 0;
  ptr_ = NULL;
  avail_ = 0;

  for (std::size_t i = 0; i < num_custom_chunks_; ++i) {
    if (custom_chunks_[i] != NULL) {
      delete [] custom_chunks_[i];
      custom_chunks_[i] = NULL;
    }
  }
  num_custom_chunks_ = 0;
}

inline String StringPool::Append(const String &str) {
  if (str.length() > avail_) {
    if (str.length() > chunk_size_) {
      return AppendCustomString(str);
    }
    AppendChunk();
  }

  String str_clone(ptr_, str.length());
  for (std::size_t i = 0; i < str.length(); ++i) {
    *ptr_++ = str[i];
  }
  avail_ -= str.length();

  ++num_strings_;
  total_length_ += str.length();
  return str_clone;
}

inline String StringPool::Append(const String &str, WithEos) {
  if (str.length() >= avail_) {
    if (str.length() >= chunk_size_) {
      return AppendCustomString(str);
    }
    AppendChunk();
  }

  String str_clone(ptr_, str.length());
  for (std::size_t i = 0; i < str.length(); ++i) {
    *ptr_++ = str[i];
  }
  *ptr_++ = '\0';
  avail_ -= str.length() + 1;

  ++num_strings_;
  total_length_ += str.length();
  return str_clone;
}

inline String StringPool::AppendCustomString(const String &str) {
  if (num_custom_chunks_ == max_num_custom_chunks_) {
    std::size_t new_max_num_custom_chunks =
      (max_num_custom_chunks_ != 0) ? (max_num_custom_chunks_ * 2) : 1;
    char **new_custom_chunks = new char *[new_max_num_custom_chunks];
    for (std::size_t i = 0; i < num_custom_chunks_; ++i) {
      new_custom_chunks[i] = custom_chunks_[i];
    }
    delete [] custom_chunks_;
    custom_chunks_ = new_custom_chunks;
    max_num_custom_chunks_ = new_max_num_custom_chunks;
  }
  char *new_custom_chunk = new char[str.length() + 1];
  custom_chunks_[num_custom_chunks_++] = new_custom_chunk;

  for (std::size_t i = 0; i < str.length(); ++i) {
    new_custom_chunk[i] = str[i];
  }
  new_custom_chunk[str.length()] = '\0';

  ++num_strings_;
  total_length_ += str.length();
  total_size_ += str.length() + 1;

  return String(new_custom_chunk, str.length());
}

inline void StringPool::AppendChunk() {
  if (chunk_id_ == num_chunks_) {
    if (num_chunks_ == max_num_chunks_) {
      std::size_t new_max_num_chunks =
        (max_num_chunks_ != 0) ? (max_num_chunks_ * 2) : 1;
      char **new_chunks = new char *[new_max_num_chunks];
      for (std::size_t i = 0; i < num_chunks_; ++i) {
        new_chunks[i] = chunks_[i];
      }
      delete [] chunks_;
      chunks_ = new_chunks;
      max_num_chunks_ = new_max_num_chunks;
    }
    char *new_chunk = new char[chunk_size_];
    chunks_[num_chunks_++] = new_chunk;
  }
  ptr_ = chunks_[chunk_id_++];
  avail_ = chunk_size_;
  total_size_ += chunk_size_;
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_STRING_POOL_H_
