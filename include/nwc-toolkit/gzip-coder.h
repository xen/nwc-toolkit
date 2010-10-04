// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_GZIP_CODER_H_
#define NWC_TOOLKIT_GZIP_CODER_H_

#include <zlib.h>

#include "./coder.h"

namespace nwc_toolkit {

class GzipCoder : public Coder {
 public:
  GzipCoder() : stream_(), mode_(NO_MODE), is_end_(false) {
    InitStream();
  }
  ~GzipCoder() {
    if (is_open()) {
      Close();
    }
  }

  bool OpenEncoder(int preset = DEFAULT_PRESET);
  bool OpenDecoder();
  bool Close();

  bool Code() {
    return Code(Z_NO_FLUSH);
  }
  bool Finish() {
    return Code(Z_FINISH);
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
    stream_.next_in = static_cast< ::Bytef *>(const_cast<void *>(next_in));
  }
  void set_avail_in(std::size_t avail_in) {
    stream_.avail_in = avail_in;
  }
  void set_next_out(void *next_out) {
    stream_.next_out = static_cast< ::Bytef *>(next_out);
  }
  void set_avail_out(std::size_t avail_out) {
    stream_.avail_out = avail_out;
  }

 private:
  ::z_stream stream_;
  Mode mode_;
  bool is_end_;

  void InitStream() {
    static const ::z_stream initial_stream = {};
    stream_ = initial_stream;
    stream_.zalloc = Z_NULL;
    stream_.zfree = Z_NULL;
    stream_.opaque = Z_NULL;
  }

  bool Code(int flush);

  // Non-copyable.
};

inline bool GzipCoder::OpenEncoder(int preset) {
  if (is_open()) {
    return false;
  }
  int level = preset;
  switch (preset) {
    case DEFAULT_PRESET: {
      level = Z_DEFAULT_COMPRESSION;
      break;
    }
    case BEST_SPEED_PRESET: {
      level = Z_BEST_SPEED;
      break;
    }
    case BEST_COMPRESSION_PRESET: {
      level = Z_BEST_COMPRESSION;
      break;
    }
    default: {
      if (preset < 0 || preset > 9) {
        return false;
      }
    }
  }
  int ret = ::deflateInit2(&stream_, level,
      Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
  if (ret != Z_OK) {
    return false;
  }
  mode_ = ENCODER_MODE;
  return true;
}

inline bool GzipCoder::OpenDecoder() {
  if (is_open()) {
    return false;
  }
  int ret = ::inflateInit2(&stream_, 47);
  if (ret != Z_OK) {
    return false;
  }
  mode_ = DECODER_MODE;
  return true;
}

inline bool GzipCoder::Close() {
  int ret = Z_OK;
  switch (mode()) {
    case ENCODER_MODE: {
      ret = ::deflateEnd(&stream_);
      break;
    }
    case DECODER_MODE: {
      ret = ::inflateEnd(&stream_);
      break;
    }
    default: {
      return false;
    }
  }
  InitStream();
  mode_ = NO_MODE;
  is_end_ = false;
  return ret >= 0;
}

inline bool GzipCoder::Code(int flush) {
  if (is_end()) {
    return false;
  }
  int ret = Z_OK;
  switch (mode()) {
    case ENCODER_MODE: {
      ret = ::deflate(&stream_, flush);
      break;
    }
    case DECODER_MODE: {
      ret = ::inflate(&stream_, flush);
      break;
    }
    default: {
      return false;
    }
  }
  if (ret == Z_STREAM_END) {
    is_end_ = true;
  }
  return (ret >= 0) || (ret == Z_BUF_ERROR);
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_GZIP_CODER_H_
