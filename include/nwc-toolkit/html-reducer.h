// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_HTML_REDUCER_H_
#define NWC_TOOLKIT_HTML_REDUCER_H_

#include "./html-document.h"

namespace nwc_toolkit {

class HtmlReducer {
 public:
  static void Reduce(const HtmlDocument &src, StringBuilder *dest);

 private:
  StringBuilder * const dest_;
  bool is_code_;

  explicit HtmlReducer(StringBuilder *dest) : dest_(dest), is_code_(false) {}
  ~HtmlReducer() {}

  void Reduce(const HtmlDocument &src);

  void ReduceText(const nwc_toolkit::HtmlUnit &unit);
  void ReduceTag(const nwc_toolkit::HtmlUnit &unit);
  void ReduceComment(const nwc_toolkit::HtmlUnit &unit);

  static bool ExtractCodePoint(const String &str,
      String *str_left, int *code_point);
  static char ReduceCodePoint(int code_point);

  // Disallows copy and assignment.
  HtmlReducer(const HtmlReducer &);
  HtmlReducer &operator=(const HtmlReducer &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HTML_REDUCER_H_
