// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CHAR_TYPE_H_
#define NWC_TOOLKIT_CHAR_TYPE_H_

#include <cctype>

#define NWC_TOOLKIT_DEFINE_CHAR_TYPE(functor_name, function_name) \
class functor_name { \
 public: \
  bool operator()(char c) const { \
    return std::function_name(static_cast<unsigned char>(c)); \
  } \
};

namespace nwc_toolkit {

NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsAlnum, isalnum)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsAlpha, isalpha)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsCntrl, iscntrl)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsDigit, isdigit)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsGraph, isgraph)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsLower, islower)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsPrint, isprint)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsPunct, ispunct)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsSpace, isspace)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsUpper, isupper)
NWC_TOOLKIT_DEFINE_CHAR_TYPE(IsXdigit, isxdigit)

}  // namespace nwc_toolkit

#undef NWC_TOOLKIT_DEFINE_CHAR_TYPE

#endif  // NWC_TOOLKIT_CHAR_TYPE_H_
