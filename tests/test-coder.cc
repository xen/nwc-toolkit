// Copyright 2010 Susumu Yata <syata@acm.org>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>

#include <nwc-toolkit/bzip2-coder.h>
#include <nwc-toolkit/gzip-coder.h>
#include <nwc-toolkit/xz-coder.h>

namespace {

void TestCode(nwc_toolkit::Coder *coder,
    const std::vector<char> &src, std::vector<char> *dest) {
  enum { CHUNK_SIZE = 100 };

  dest->resize(CHUNK_SIZE);

  coder->set_next_in(&src[0]);
  coder->set_avail_in(src.size());
  coder->set_next_out(&(*dest)[0]);
  coder->set_avail_out(dest->size());

  while (coder->total_in() < src.size()) {
    if (coder->avail_in() == 0) {
      coder->set_next_in(&src[coder->total_in()]);
      std::size_t avail_out = src.size() - coder->total_in();
      if (avail_out > CHUNK_SIZE) {
        avail_out = CHUNK_SIZE;
      }
      coder->set_avail_out(avail_out);
    }
    if (coder->avail_out() == 0) {
      dest->resize(dest->size() + CHUNK_SIZE);
      coder->set_next_out(&(*dest)[coder->total_out()]);
      coder->set_avail_out(dest->size() - coder->total_out());
    }
    assert(coder->Code());
  }
  while (!coder->is_end()) {
    assert(coder->Finish());
    if (coder->avail_out() == 0) {
      dest->resize(dest->size() + CHUNK_SIZE);
      coder->set_next_out(&(*dest)[coder->total_out()]);
      coder->set_avail_out(dest->size() - coder->total_out());
    }
  }
  assert(coder->is_end());
  assert(coder->avail_in() == 0);
  dest->resize(coder->total_out());
  assert(!coder->Code());
  assert(!coder->Finish());
  assert(coder->Close());
  assert(coder->mode() == nwc_toolkit::Coder::NO_MODE);
}

template <typename Coder>
void TestCoder(int preset, const std::vector<char> &data,
    const char *path) {
  Coder coder;
  assert(coder.mode() == nwc_toolkit::Coder::NO_MODE);

  assert(coder.OpenEncoder(preset));
  assert(coder.mode() == nwc_toolkit::Coder::ENCODER_MODE);
  assert(coder.is_open());

  std::vector<char> encoded_data;
  TestCode(&coder, data, &encoded_data);

  std::ofstream file(path, std::ios::binary);
  file.write(&encoded_data[0], encoded_data.size());
  assert(file.good());
  file.close();

  assert(coder.OpenDecoder());
  assert(coder.mode() == nwc_toolkit::Coder::DECODER_MODE);
  assert(coder.is_open());

  std::vector<char> decoded_data;
  TestCode(&coder, encoded_data, &decoded_data);

  assert(data == decoded_data);

  std::cerr << "ok (" << std::fixed << std::setprecision(2)
            << (100.0 * encoded_data.size() / data.size()) << "%)";
}

template <typename Coder>
void TestCoder(const std::vector<char> &data, const char *suffix) {
  std::cerr << "default: ";
  TestCoder<Coder>(nwc_toolkit::Coder::DEFAULT_PRESET, data, suffix);
  std::cerr << ", speed: ";
  TestCoder<Coder>(nwc_toolkit::Coder::BEST_SPEED_PRESET, data, suffix);
  std::cerr << ", compression: ";
  TestCoder<Coder>(nwc_toolkit::Coder::BEST_COMPRESSION_PRESET, data, suffix);
  std::cerr << std::endl;
}

}  // namespace

int main() {
  enum { BUF_SIZE = 1 << 16 };

  std::srand(static_cast<unsigned int>(std::time(NULL)));

  std::vector<char> data(BUF_SIZE);
  for (std::size_t i = 0; i < data.size(); ++i) {
    data[i] = 'A' + (std::rand() % 26);
  }

  std::ofstream file("test-coder.dat", std::ios::binary);
  file.write(&data[0], data.size());
  assert(file.good());
  file.close();

  std::cerr << "gzip: ";
  TestCoder<nwc_toolkit::GzipCoder>(data, "test-coder.dat.gz");
  std::cerr << "bzip2: ";
  TestCoder<nwc_toolkit::Bzip2Coder>(data, "test-coder.dat.bz2");
  std::cerr << "xz: ";
  TestCoder<nwc_toolkit::XzCoder>(data, "test-coder.dat.xz");

  return 0;
}
