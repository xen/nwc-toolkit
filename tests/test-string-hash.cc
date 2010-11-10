// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>
#include <ctime>
#include <string>
#include <tr1/memory>
#include <tr1/random>
#include <tr1/unordered_set>
#include <vector>

#include <nwc-toolkit/string-hash.h>

namespace {

std::tr1::mt19937 mt_rand(static_cast<unsigned int>(time(NULL)));

}  // namespace

int main() {
  enum { KEY_LENGTH = 16, MAX_NUM_KEYS = 4096 };

  typedef std::tr1::unordered_set<nwc_toolkit::String,
    nwc_toolkit::StringHash> KeySet;

  std::vector<std::tr1::shared_ptr<std::string> > key_pool;
  std::vector<nwc_toolkit::String> keys;
  KeySet key_set;

  char key_buf[KEY_LENGTH + 1] = "";
  nwc_toolkit::String key(key_buf, KEY_LENGTH);
  while (key_set.size() < MAX_NUM_KEYS) {
    for (std::size_t i = 0; i < KEY_LENGTH; ++i) {
      key_buf[i] = 'A' + (mt_rand() % 26);
    }
    KeySet::iterator it = key_set.find(key);
    if (it == key_set.end()) {
      std::tr1::shared_ptr<std::string> key_str(new std::string(
        key.ptr(), key.length()));
      key_pool.push_back(key_str);

      nwc_toolkit::String key_clone(key_str->c_str(), key_str->length());
      keys.push_back(key_clone);
      key_set.insert(key_clone);
    }
    assert(key_pool.size() == key_set.size());
  }

  for (std::size_t i = 0; i < keys.size(); ++i) {
    assert(key_set.find(keys[i]) != key_set.end());
  }

  return 0;
}
