// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CHAR_FILTER_H_
#define NWC_TOOLKIT_CHAR_FILTER_H_

#include <cctype>

#define NWC_TOOLKIT_DEFINE_FILTER(functor_name, function_name) \
class functor_name { \
 public: \
  char operator()(unsigned char c) const { \
    return static_cast<char>(std::function_name(c)); \
  } \
};

namespace nwc_toolkit {

class KeepAsIs
{
 public:
  char operator()(unsigned char c) const {
    return c;
  }
};

NWC_TOOLKIT_DEFINE_FILTER(ToLower, tolower)
NWC_TOOLKIT_DEFINE_FILTER(ToUpper, toupper)

}  // namespace nwc_toolkit

#undef NWC_TOOLKIT_DEFINE_FILTER

#endif  // NWC_TOOLKIT_CHAR_FILTER_H_
