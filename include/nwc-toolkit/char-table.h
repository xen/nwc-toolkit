// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CHAR_TABLE_H_
#define NWC_TOOLKIT_CHAR_TABLE_H_

#include <cstddef>

namespace nwc_toolkit {

class CharTable {
 public:
  explicit CharTable(const char *str) : table_() { Build(str); }
  CharTable(const char *str, std::size_t length)
    : table_() { Build(str, length); }
  ~CharTable() {}

  void Clear() {
    for (std::size_t i = 0; i < TABLE_SIZE; ++i) {
      table_[i] = 0;
    }
  }

  void Build(const char *str) {
    std::size_t length = 0;
    while (str[length] != '\0') {
      ++length;
    }
    Build(str, length);
  }
  void Build(const char *str, std::size_t length);

  bool Get(unsigned char c) const {
    return (table_[c / BITS_PER_INT] & (1U << (c % BITS_PER_INT))) != 0;
  }
  void Set(unsigned char c) {
    table_[c / BITS_PER_INT] |= 1U << (c % BITS_PER_INT);
  }
  void Unset(unsigned char c) {
    table_[c / BITS_PER_INT] &= ~(1U << (c % BITS_PER_INT));
  }
  void Toggle(unsigned char c) {
    table_[c / BITS_PER_INT] ^= 1U << (c % BITS_PER_INT);
  }

 private:
  enum { BITS_PER_INT = sizeof(unsigned) * 8 };
  enum { TABLE_SIZE = 256 / BITS_PER_INT };

  unsigned table_[TABLE_SIZE];

  // Disallows copy and assignment.
  CharTable(const CharTable &);
  CharTable &operator=(const CharTable &);
};

inline void CharTable::Build(const char *str, std::size_t length) {
  Clear();

  std::size_t index = 0;
  if (length > 0 && str[index] == '-') {
    Set('-');
    ++index;
  }

  while (index < length) {
    if (str[index] == '-' && index + 1 < length) {
      int c_min = static_cast<unsigned char>(str[index - 1]);
      int c_max = static_cast<unsigned char>(str[index + 1]);
      if (c_min > c_max) {
        int temp = c_min;
        c_min = c_max;
        c_max = temp;
      }

      for (int c = c_min + 1; c < c_max; ++c) {
        Set(c);
      }
      ++index;
    }
    Set(str[index]);
    ++index;
  }
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CHAR_TABLE_H_
