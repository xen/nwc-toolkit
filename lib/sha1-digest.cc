// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/sha1-digest.h>

#include <cstring>

namespace nwc_toolkit {

Sha1Digest::Sha1Digest()
    : digest_(),
      hex_digest_(),
      length_(0),
      block_() {
  Clear();
}

void Sha1Digest::Clear() {
  digest_[0] = 0x67452301;
  digest_[1] = 0xEFCDAB89;
  digest_[2] = 0x98BADCFE;
  digest_[3] = 0x10325476;
  digest_[4] = 0xC3D2E1F0;
  std::memset(hex_digest_, 0, sizeof(hex_digest_));
  length_ = 0;
  std::memset(block_, 0, sizeof(block_));
}

void Sha1Digest::Append(const String &message) {
  std::size_t avail_in = message.length();
  const char *next_in = message.ptr();

  unsigned int avail_out = static_cast<unsigned int>(64 - (length_ % 64));
  unsigned char *next_out = block_ + 64 - avail_out;

  while (avail_in != 0) {
    *next_out = static_cast<unsigned char>(*next_in);
    --avail_in;
    ++next_in;
    --avail_out;
    if (avail_out == 0) {
      ProcessBlock();
      avail_out = 64;
      next_out = block_;
    } else {
      ++next_out;
    }
  }
  length_ += message.length();
}

void Sha1Digest::Finish() {
  unsigned int index = static_cast<unsigned int>(length_ % 64);
  block_[index++] = 0x80;
  if (index > 56) {
    memset(block_ + index, 0, 64 - index);
    ProcessBlock();
    index = 0;
  }

  memset(block_ + index, 0, 56 - index);
  for (index = 56; index < 64; ++index) {
    block_[index] = static_cast<unsigned char>(
        (length_ * 8) >> ((63 - index) * 8));
  }
  ProcessBlock();

  char *next_hex = hex_digest_;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 8; ++j) {
      *next_hex++ = ToHex((digest_[i] >> ((7 - j) * 4)) & 0x0F);
    }
  }
}

void Sha1Digest::ProcessBlock() {
  static const unsigned int MAGIC_NUMBERS[] = {
    0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
  };

  unsigned int buf[85];
  for (int i = 0; i < 5; ++i) {
    buf[i] = digest_[4 - i];
  }
  for (int i = 0; i < 16; ++i) {
    buf[i + 5] = (block_[(i * 4)] << 24) | (block_[(i * 4) + 1] << 16)
        | (block_[(i * 4) + 2] << 8) | (block_[(i * 4) + 3]);
  }
  for (int i = 16; i < 80; ++i) {
    buf[i + 5] = Shift(1, buf[i + 2] ^ buf[i - 3] ^ buf[i - 9] ^ buf[i - 11]);
  }

  unsigned int *w = buf;
  for (int i = 0; i < 20; ++i, ++w) {
    w[5] += Shift(5, w[4]) + ((w[3] & w[2]) | ((~w[3]) & w[1]))
        + w[0] + MAGIC_NUMBERS[0];
    w[3] = Shift(30, w[3]);
  }
  for (int i = 0; i < 20; ++i, ++w) {
    w[5] += Shift(5, w[4]) + (w[3] ^ w[2] ^ w[1]) + w[0] + MAGIC_NUMBERS[1];
    w[3] = Shift(30, w[3]);
  }
  for (int i = 0; i < 20; ++i, ++w) {
    w[5] += Shift(5, w[4]) + ((w[3] & w[2]) | (w[3] & w[1])
        | (w[2] & w[1])) + w[0] + MAGIC_NUMBERS[2];
    w[3] = Shift(30, w[3]);
  }
  for (int i = 0; i < 20; ++i, ++w) {
    w[5] += Shift(5, w[4]) + (w[3] ^ w[2] ^ w[1]) + w[0] + MAGIC_NUMBERS[3];
    w[3] = Shift(30, w[3]);
  }

  for (int i = 0; i < 5; ++i) {
    digest_[i] += w[4 - i];
  }
}

inline unsigned int Sha1Digest::Shift(int num_bits, unsigned int word) {
  return (word << num_bits) | (word >> (32 - num_bits));
}

inline char Sha1Digest::ToHex(unsigned int value) {
  return (value < 10) ? (value + '0') : (value - 10 + 'a');
}

}  // namespace nwc_toolkit
