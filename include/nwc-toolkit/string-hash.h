// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_STRING_HASH_H_
#define NWC_TOOLKIT_STRING_HASH_H_

#include "./string.h"

namespace nwc_toolkit {

class StringHash {
 public:
  unsigned int operator()(const String &str) const;

 private:
  static void Mix(unsigned int &x, unsigned int &y, unsigned int &z);

  static unsigned int ToUInt32(unsigned char c) {
    return c;
  }
};

// http://burtleburtle.net/bob/hash/doobs.html
inline unsigned int StringHash::operator()(const String &str) const {
  unsigned int x = 0x9E3779B9U;
  unsigned int y = 0x9E3779B9U;
  unsigned int z = 0;

  String avail = str;
  while (avail.length() >= 12) {
    x += ToUInt32(avail[0]) + (ToUInt32(avail[1]) << 8)
      + (ToUInt32(avail[2]) << 16) + (ToUInt32(avail[3]) << 24);
    y += ToUInt32(avail[4]) + (ToUInt32(avail[5]) << 8)
      + (ToUInt32(avail[6]) << 16) + (ToUInt32(avail[7]) << 24);
    z += ToUInt32(avail[8]) + (ToUInt32(avail[9]) << 8)
      + (ToUInt32(avail[10]) << 16) + (ToUInt32(avail[11]) << 24);
    Mix(x, y, z);
    avail = avail.SubString(12);
  }

  z += str.length();
  switch (avail.length()) {
  case 11:
    z += ToUInt32(avail[10]) << 24;
  case 10:
    z += ToUInt32(avail[9]) << 16;
  case 9:
    z += ToUInt32(avail[8]) << 8;
  case 8:
    y += ToUInt32(avail[7]) << 24;
  case 7:
    y += ToUInt32(avail[6]) << 16;
  case 6:
    y += ToUInt32(avail[5]) << 8;
  case 5:
    y += ToUInt32(avail[4]);
  case 4:
    x += ToUInt32(avail[3]) << 24;
  case 3:
    x += ToUInt32(avail[2]) << 16;
  case 2:
    x += ToUInt32(avail[1]) << 8;
  case 1:
    x += ToUInt32(avail[0]);
  }

  Mix(x, y, z);
  return z;
}

inline void StringHash::Mix(unsigned int &x, unsigned int &y,
  unsigned int &z) {
  x -= y; x -= z; x ^= (z >> 13);
  y -= z; y -= x; y ^= (x << 8);
  z -= x; z -= y; z ^= (y >> 13);
  x -= y; x -= z; x ^= (z >> 12);
  y -= z; y -= x; y ^= (x << 16);
  z -= x; z -= y; z ^= (y >> 5);
  x -= y; x -= z; x ^= (z >> 3);
  y -= z; y -= x; y ^= (x << 10);
  z -= x; z -= y; z ^= (y >> 15);
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_STRING_HASH_H_
