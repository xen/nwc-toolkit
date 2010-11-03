// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_SHA1_DIGEST_H_
#define NWC_TOOLKIT_SHA1_DIGEST_H_

#include "./string.h"

namespace nwc_toolkit {

class Sha1Digest {
 public:
  Sha1Digest();
  ~Sha1Digest() {}

  String digest() const {
    return String(reinterpret_cast<const char *>(digest_), 20);
  }
  String hex_digest() const {
    return String(hex_digest_, 40);
  }
  unsigned long long length() const {
    return length_;
  }

  void Clear();
  void Append(const String &message);
  void Finish();

 private:
  unsigned int digest_[5];
  char hex_digest_[40];
  unsigned long long length_;
  unsigned char block_[64];

  void ProcessBlock();

  inline static unsigned int Shift(int num_bits, unsigned int word);
  inline static char ToHex(unsigned int value);

  // Disallows copy and assignment.
  Sha1Digest(const Sha1Digest &);
  Sha1Digest &operator=(const Sha1Digest &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_SHA1_DIGEST_H_
