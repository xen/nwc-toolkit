// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>
#include <ctime>
#include <tr1/random>
#include <vector>

#include <nwc-toolkit/string-pool.h>

namespace {

std::tr1::mt19937 mt_rand(static_cast<unsigned int>(time(NULL)));

}  // namespace

int main() {
  enum { CHUNK_SIZE = 1024, POOL_SIZE = 1024 };

  nwc_toolkit::StringPool pool(CHUNK_SIZE);

  assert(pool.chunk_size() == 1024);
  assert(pool.num_strings() == 0);
  assert(pool.total_length() == 0);
  assert(pool.total_size() == 0);

  char buf[CHUNK_SIZE * 2];
  for (std::size_t i = 0; i < sizeof(buf); ++i) {
    buf[i] = 'A' + (mt_rand() % 26);
  }

  std::size_t num_strings = 0;
  std::size_t total_length = 0;
  std::size_t total_size = 0;
  std::size_t avail = 0;
  std::size_t num_chunks = 0;

  std::vector<nwc_toolkit::String> originals;
  std::vector<nwc_toolkit::String> copies;

  for (std::size_t i = 0; i < POOL_SIZE; ++i) {
    int pos = mt_rand() % sizeof(buf);
    std::size_t length = mt_rand() % (sizeof(buf) - pos);

    nwc_toolkit::String str(buf + pos, length);
    nwc_toolkit::String str_copy = pool.Append(str);

    assert(str == str_copy);
    assert(str.ptr() != str_copy.ptr());

    originals.push_back(str);
    copies.push_back(str_copy);

    ++num_strings;
    assert(pool.num_strings() == num_strings);

    total_length += length;
    assert(pool.total_length() == total_length);

    if (length > CHUNK_SIZE) {
      total_size += length + 1;
    } else {
      if (length > avail) {
        total_size += CHUNK_SIZE;
        avail = CHUNK_SIZE;
        ++num_chunks;
      }
      avail -= length;
    }
    assert(pool.total_size() == total_size);
  }

  for (std::size_t i = 0; i < POOL_SIZE; ++i) {
    assert(originals[i] == copies[i]);
  }

  for (std::size_t i = 0; i < POOL_SIZE; ++i) {
    int pos = mt_rand() % sizeof(buf);
    std::size_t length = mt_rand() % (sizeof(buf) - pos);

    nwc_toolkit::String str(buf + pos, length);
    nwc_toolkit::String str_copy =
      pool.Append(str, nwc_toolkit::StringPool::WithEos());

    assert(str == str_copy);
    assert(str.ptr() != str_copy.ptr());

    originals.push_back(str);
    copies.push_back(str_copy);

    ++num_strings;
    assert(pool.num_strings() == num_strings);

    total_length += length;
    assert(pool.total_length() == total_length);

    if (length >= CHUNK_SIZE) {
      total_size += length + 1;
    } else {
      if (length >= avail) {
        total_size += CHUNK_SIZE;
        avail = CHUNK_SIZE;
        ++num_chunks;
      }
      avail -= length + 1;
    }
    assert(pool.total_size() == total_size);
  }

  for (std::size_t i = 0; i < POOL_SIZE; ++i) {
    assert(originals[i] == copies[i]);
  }

  pool.Clear();

  assert(pool.num_strings() == 0);
  assert(pool.total_length() == 0);
  assert(pool.total_size() == (num_chunks * pool.chunk_size()));

  return 0;
}
