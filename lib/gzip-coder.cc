// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/gzip-coder.h>

namespace nwc_toolkit {

bool GzipCoder::OpenEncoder(int preset) {
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

bool GzipCoder::OpenDecoder() {
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

bool GzipCoder::Close() {
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

bool GzipCoder::Code(int flush) {
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
  return (ret >= 0) || (ret == Z_BUF_ERROR) || (ret == Z_STREAM_ERROR);
}

}  // namespace nwc_toolkit
