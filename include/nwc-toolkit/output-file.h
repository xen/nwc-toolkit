// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_OUTPUT_FILE_H_
#define NWC_TOOLKIT_OUTPUT_FILE_H_

#include <cstdio>
#include <tr1/memory>

#include "./bzip2-coder.h"
#include "./gzip-coder.h"
#include "./string-builder.h"
#include "./xz-coder.h"

namespace nwc_toolkit {

class OutputFile {
 public:
  enum { DEFAULT_IO_BUF_SIZE = 1 << 18 };

  OutputFile() : file_(NULL), coder_(), io_buf_() {}
  ~OutputFile() {
    if (is_open()) {
      Close();
    }
  }

  bool Open(const String &path, std::size_t io_buf_size = 0,
      int coder_preset = Coder::DEFAULT_PRESET);
  bool Close();

  bool is_open() const {
    return file_ != NULL;
  }

  bool Write(const String &str);

 private:
  FILE *file_;
  std::tr1::shared_ptr<Coder> coder_;
  StringBuilder io_buf_;

  // Disallows copy and assignment.
  OutputFile(const OutputFile &);
  OutputFile &operator=(const OutputFile &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_OUTPUT_FILE_H_
