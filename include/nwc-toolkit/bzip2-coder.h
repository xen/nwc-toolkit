// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_BZIP2_CODER_H_
#define NWC_TOOLKIT_BZIP2_CODER_H_

#include <bzlib.h>

#include "./coder.h"

namespace nwc_toolkit {

class Bzip2Coder : public Coder {
 public:
  Bzip2Coder() : stream_(), mode_(NO_MODE), is_end_(false) {
    InitStream();
  }
  ~Bzip2Coder() {
    if (is_open()) {
      Close();
    }
  }

  bool OpenEncoder(int preset = DEFAULT_PRESET);
  bool OpenDecoder();
  bool Close();

  bool Code() {
    return Code(BZ_RUN);
  }
  bool Finish() {
    return Code(BZ_FINISH);
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
    return ((0ULL + stream_.total_in_hi32) << 32) | stream_.total_in_lo32;
  }
  void *next_out() const {
    return stream_.next_out;
  }
  std::size_t avail_out() const {
    return stream_.avail_out;
  }
  unsigned long long total_out() const {
    return ((0ULL + stream_.total_out_hi32) << 32) | stream_.total_out_lo32;
  }

  void set_next_in(const void *next_in) {
    stream_.next_in = static_cast<char *>(const_cast<void *>(next_in));
  }
  void set_avail_in(std::size_t avail_in) {
    stream_.avail_in = avail_in;
  }
  void set_next_out(void *next_out) {
    stream_.next_out = static_cast<char *>(next_out);
  }
  void set_avail_out(std::size_t avail_out) {
    stream_.avail_out = avail_out;
  }

 private:
  ::bz_stream stream_;
  Mode mode_;
  bool is_end_;

  void InitStream() {
    static const ::bz_stream initial_stream = {};
    stream_ = initial_stream;
    stream_.bzalloc = NULL;
    stream_.bzfree = NULL;
    stream_.opaque = NULL;
  }

  bool Code(int flush);

  // Non-copyable.
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_BZIP2_CODER_H_
