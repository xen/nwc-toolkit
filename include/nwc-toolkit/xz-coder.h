// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_XZ_CODER_H_
#define NWC_TOOLKIT_XZ_CODER_H_

#include <cstdint>

#include <lzma.h>

#include "./coder.h"

namespace nwc_toolkit {

class XzCoder : public Coder {
 public:
  XzCoder() : stream_(), mode_(NO_MODE), is_end_(false) {
    InitStream();
  }
  ~XzCoder() {
    if (is_open()) {
      Close();
    }
  }

  bool OpenEncoder(int preset = DEFAULT_PRESET);
  bool OpenDecoder();
  bool Close();

  bool Code() {
    return Code(LZMA_RUN);
  }
  bool Finish() {
    return Code(LZMA_FINISH);
  }

  Mode mode() const {
    return mode_;
  }
  bool is_open() const {
    return mode() != NO_MODE;
  }
  bool is_end() const {
    return is_end_;
  }

  const void *next_in() const {
    return stream_.next_in;
  }
  std::size_t avail_in() const {
    return stream_.avail_in;
  }
  unsigned long long total_in() const {
    return stream_.total_in;
  }
  void *next_out() const {
    return stream_.next_out;
  }
  std::size_t avail_out() const {
    return stream_.avail_out;
  }
  unsigned long long total_out() const {
    return stream_.total_out;
  }

  void set_next_in(const void *next_in) {
    stream_.next_in = static_cast<const std::uint8_t *>(next_in);
  }
  void set_avail_in(std::size_t avail_in) {
    stream_.avail_in = avail_in;
  }
  void set_next_out(void *next_out) {
    stream_.next_out = static_cast<std::uint8_t *>(next_out);
  }
  void set_avail_out(std::size_t avail_out) {
    stream_.avail_out = avail_out;
  }

 private:
  ::lzma_stream stream_;
  Mode mode_;
  bool is_end_;

  void InitStream() {
    static const ::lzma_stream initial_stream = LZMA_STREAM_INIT;
    stream_ = initial_stream;
  }

  bool Code(::lzma_action action);

  // Non-copyable.
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_XZ_CODER_H_
