// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_HTML_ARCHIVE_ENTRY_H_
#define NWC_TOOLKIT_HTML_ARCHIVE_ENTRY_H_

#include "./input-file.h"
#include "./output-file.h"
#include "./string-builder.h"

namespace nwc_toolkit {

class HtmlArchiveEntry {
 public:
  HtmlArchiveEntry() : url_(), status_code_(0), header_(), body_() {}
  ~HtmlArchiveEntry() { Clear(); }

  String url() const { return url_.str(); }
  int status_code() const { return status_code_; }
  String header() const { return header_.str(); }
  String body() const { return body_.str(); }

  void set_url(const String &url) { url_.Assign(url).Append(); }
  void set_status_code(int status_code) { status_code_ = status_code; }
  void set_header(const String &header) { header_.Assign(header).Append(); }
  void set_body(const String &body) { body_.Assign(body).Append(); }

  void Clear();

  bool Read(InputFile *file);
  bool Write(OutputFile *file) const;

 private:
  StringBuilder url_;
  int status_code_;
  StringBuilder header_;
  StringBuilder body_;

  static bool ReadInt(InputFile *file, int *value);
  static bool WriteInt(int value, OutputFile *file);

  // Disallows copy and assignment.
  HtmlArchiveEntry(const HtmlArchiveEntry &);
  HtmlArchiveEntry &operator=(const HtmlArchiveEntry &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HTML_ARCHIVE_ENTRY_H_
