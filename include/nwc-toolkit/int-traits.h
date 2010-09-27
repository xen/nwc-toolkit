// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_INT_TRAITS_H_
#define NWC_TOOLKIT_INT_TRAITS_H_

#include <cstddef>

#define NWC_TOOLKIT_DEFINE_INT_TRAITS(type) \
template <> \
class IntTraits<type> { \
 public: \
  typedef IsInt Tag; \
};

namespace nwc_toolkit {

class IsInt {
 public:
  static bool is_int() { return true; }
};

class IsNotInt {
 public:
  static bool is_int() { return false; }
};

template <typename T>
class IntTraits {
 public:
  typedef IsNotInt Tag;
};

NWC_TOOLKIT_DEFINE_INT_TRAITS(char)
NWC_TOOLKIT_DEFINE_INT_TRAITS(signed char)
NWC_TOOLKIT_DEFINE_INT_TRAITS(unsigned char)
NWC_TOOLKIT_DEFINE_INT_TRAITS(short)
NWC_TOOLKIT_DEFINE_INT_TRAITS(unsigned short)
NWC_TOOLKIT_DEFINE_INT_TRAITS(int)
NWC_TOOLKIT_DEFINE_INT_TRAITS(unsigned int)
NWC_TOOLKIT_DEFINE_INT_TRAITS(long)
NWC_TOOLKIT_DEFINE_INT_TRAITS(unsigned long)
NWC_TOOLKIT_DEFINE_INT_TRAITS(long long)
NWC_TOOLKIT_DEFINE_INT_TRAITS(unsigned long long)
NWC_TOOLKIT_DEFINE_INT_TRAITS(wchar_t)

}  // namespace nwc_toolkit

#undef NWC_TOOLKIT_DEFINE_INT_TRAITS

#endif  // NWC_TOOLKIT_INT_TRAITS_H_
