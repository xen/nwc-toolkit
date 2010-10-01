// Copyright 2010 Susumu Yata <syata@acm.org>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include <nwc-toolkit/xz-coder.h>

namespace {

void TestCodeAtOnce(nwc_toolkit::Coder *coder,
    const std::vector<char> &src, std::vector<char> *dest) {
  enum { CHUNK_SIZE = 100 };

  dest->resize(CHUNK_SIZE);

  coder->set_next_in(&src[0]);
  coder->set_avail_in(src.size());
  coder->set_next_out(&(*dest)[0]);
  coder->set_avail_out(dest->size());

  while (coder->avail_in() > 0) {
    if (coder->avail_out() == 0) {
      dest->resize(dest->size() + CHUNK_SIZE);
      coder->set_next_out(&(*dest)[coder->total_out()]);
      coder->set_avail_out(dest->size() - coder->total_out());
    }
    assert(coder->Code());
  }
  while (!coder->is_end()) {
    if (coder->avail_out() == 0) {
      dest->resize(dest->size() + CHUNK_SIZE);
      coder->set_next_out(&(*dest)[coder->total_out()]);
      coder->set_avail_out(dest->size() - coder->total_out());
    }
    assert(coder->Finish());
  }
  assert(coder->is_end());
  assert(coder->avail_in() == 0);
  dest->resize(coder->total_out());
  assert(!coder->Code());
  assert(!coder->Finish());
  assert(coder->Close());
  assert(coder->mode() == nwc_toolkit::Coder::NO_MODE);
}

void TestCodeTimes(nwc_toolkit::Coder *coder,
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

}  // namespace

int main() {
  enum { BUF_SIZE = 1 << 16 };

  std::srand(static_cast<unsigned int>(std::time(NULL)));

  std::vector<char> raw_data(BUF_SIZE);
  for (std::size_t i = 0; i < raw_data.size(); ++i) {
    raw_data[i] = 'A' + (std::rand() % 26);
  }

  std::ofstream file;
  file.open("test-xz-coder.dat", std::ios::binary);
  file.write(&raw_data[0], raw_data.size());
  file.close();

  std::vector<char> encoded_data;
  std::vector<char> decoded_data;

  nwc_toolkit::XzCoder coder;
  assert(coder.mode() == nwc_toolkit::Coder::NO_MODE);

  assert(coder.OpenEncoder());
  assert(coder.mode() == nwc_toolkit::Coder::ENCODER_MODE);
  TestCodeAtOnce(&coder, raw_data, &encoded_data);

  file.open("test-xz-coder.dat.xz", std::ios::binary);
  file.write(&encoded_data[0], encoded_data.size());
  file.close();

  assert(coder.OpenDecoder());
  assert(coder.mode() == nwc_toolkit::Coder::DECODER_MODE);
  TestCodeAtOnce(&coder, encoded_data, &decoded_data);

  assert(raw_data == decoded_data);

  std::vector<char> buf;

  assert(coder.OpenEncoder());
  assert(coder.mode() == nwc_toolkit::Coder::ENCODER_MODE);
  TestCodeTimes(&coder, decoded_data, &buf);

  assert(buf == encoded_data);

  assert(coder.OpenDecoder());
  assert(coder.mode() == nwc_toolkit::Coder::DECODER_MODE);
  TestCodeTimes(&coder, encoded_data, &buf);

  assert(buf == decoded_data);

  return 0;
}
