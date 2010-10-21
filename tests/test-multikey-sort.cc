// Copyright 2010 Susumu Yata <syata@acm.org>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>

#include <nwc-toolkit/multikey-sort.h>

namespace {

template <typename T>
class LessThan {
 public:
  bool operator()(const T *lhs, const T *rhs) {
    while ((*lhs != 0) && (*lhs == *rhs)) {
      ++lhs, ++rhs;
    }
    return *lhs < *rhs;
  }
};

template <>
class LessThan<char> {
 public:
  bool operator()(const char *lhs, const char *rhs) {
    while ((*lhs != '\0') && (*lhs == *rhs)) {
      ++lhs, ++rhs;
    }
    return static_cast<unsigned char>(*lhs)
        < static_cast<unsigned char>(*rhs);
  }
};

template <typename T>
void TestMultikeySort() {
  enum { KEY_LENGTH = 8 };
  enum { NUM_KEYS = 1 << 16 };

  std::vector<T *> keys;
  for (std::size_t i = 0; i < NUM_KEYS; ++i) {
    T *key = new T[KEY_LENGTH + 1];
    for (std::size_t j = 0; j < KEY_LENGTH; ++j) {
      key[j] = static_cast<T>(std::rand());
    }
    key[KEY_LENGTH] = 0;
    keys.push_back(key);
  }

  std::vector<T *> sorted_keys(keys);
  std::sort(sorted_keys.begin(), sorted_keys.end(), LessThan<T>());

  std::vector<T *> multikey_sorted_keys(keys);
  assert(nwc_toolkit::MultikeySort(multikey_sorted_keys.begin(),
      multikey_sorted_keys.end()));

  for (std::size_t i = 0; i < keys.size(); ++i) {
    assert(!LessThan<T>()(sorted_keys[i], multikey_sorted_keys[i]));
    assert(!LessThan<T>()(multikey_sorted_keys[i], sorted_keys[i]));
  }

  for (std::size_t i = 0; i < keys.size(); ++i) {
    delete [] keys[i];
    keys[i] = NULL;
  }
}

}  // namespace

int main() {
  std::srand(static_cast<unsigned int>(std::time(NULL)));

  const char *keys[] = {
    "melon", "orange", "apple", "banana"
  };
  std::size_t num_keys = sizeof(keys) / sizeof(*keys);

  assert(nwc_toolkit::MultikeySort(keys, keys + num_keys));
  assert(std::strcmp(keys[0], "apple") == 0);
  assert(std::strcmp(keys[1], "banana") == 0);
  assert(std::strcmp(keys[2], "melon") == 0);
  assert(std::strcmp(keys[3], "orange") == 0);

  std::cerr << "char: ";
  TestMultikeySort<char>();
  std::cerr << "ok, short: ";
  TestMultikeySort<short>();
  std::cerr << "ok, int: ";
  TestMultikeySort<int>();
  std::cerr << "ok, long: ";
  TestMultikeySort<long>();
  std::cerr << "ok, long long: ";
  TestMultikeySort<long long>();
  std::cerr << "ok" << std::endl;

  std::cerr << "(unsigned) char: ";
  TestMultikeySort<unsigned char>();
  std::cerr << "ok, short: ";
  TestMultikeySort<unsigned short>();
  std::cerr << "ok, int: ";
  TestMultikeySort<unsigned int>();
  std::cerr << "ok, long: ";
  TestMultikeySort<unsigned long>();
  std::cerr << "ok, long long: ";
  TestMultikeySort<unsigned long long>();
  std::cerr << "ok" << std::endl;

  return 0;
}
