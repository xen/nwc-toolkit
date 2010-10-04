// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_INPUT_FILE_H_
#define NWC_TOOLKIT_INPUT_FILE_H_

#include <cstdio>
#include <memory>

#include "./bzip2-coder.h"
#include "./gzip-coder.h"
#include "./string-builder.h"
#include "./xz-coder.h"

namespace nwc_toolkit {

class InputFile {
 public:
  enum {
    DEFAULT_CODER_BUF_SIZE = 1 << 10,
    DEFAULT_IO_BUF_SIZE = 1 << 18
  };

  InputFile()
      : file_(NULL),
        coder_(),
        io_buf_(),
        coder_buf_(),
        next_(NULL),
        avail_(0) {}
  ~InputFile() {
    if (is_open()) {
      Close();
    }
  }

  bool Open(const String &path, std::size_t io_buf_size = 0,
      std::size_t coder_buf_size = 0);
  bool Close();

  bool is_open() const {
    return file_ != NULL;
  }

  bool Read(std::size_t size, String *data);

  bool ReadLine(String *line) {
    return ReadLine('\n', line);
  }
  bool ReadLine(char delim, String *line);

 private:
  FILE *file_;
  std::shared_ptr<Coder> coder_;
  StringBuilder io_buf_;
  StringBuilder coder_buf_;
  const char *next_;
  std::size_t avail_;

  void ShiftToFront(std::size_t request_size);
  bool FillBuf();

  // Disallows copy and assignment.
  InputFile(const InputFile &);
  InputFile &operator=(const InputFile &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_INPUT_FILE_H_
