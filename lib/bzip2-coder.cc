// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/bzip2-coder.h>

namespace nwc_toolkit {

bool Bzip2Coder::OpenEncoder(int preset) {
  if (is_open()) {
    return false;
  }
  int block_size = preset;
  switch (preset) {
    case DEFAULT_PRESET: {
      block_size = 6;
      break;
    }
    case BEST_SPEED_PRESET: {
      block_size = 1;
      break;
    }
    case BEST_COMPRESSION_PRESET: {
      block_size = 9;
      break;
    }
    default: {
      if (preset < 1 || preset > 9) {
        return false;
      }
    }
  }
  int ret = ::BZ2_bzCompressInit(&stream_, block_size, 0, 0);
  if (ret != BZ_OK) {
    return false;
  }
  mode_ = ENCODER_MODE;
  return true;
}

bool Bzip2Coder::OpenDecoder() {
  if (is_open()) {
    return false;
  }
  int ret = ::BZ2_bzDecompressInit(&stream_, 0, 0);
  if (ret != BZ_OK) {
    return false;
  }
  mode_ = DECODER_MODE;
  return true;
}

bool Bzip2Coder::Close() {
  int ret = BZ_OK;
  switch (mode()) {
    case ENCODER_MODE: {
      ret = ::BZ2_bzCompressEnd(&stream_);
      break;
    }
    case DECODER_MODE: {
      ret = ::BZ2_bzDecompressEnd(&stream_);
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

bool Bzip2Coder::Code(int action) {
  if (is_end()) {
    return false;
  }
  int ret = BZ_OK;
  switch (mode()) {
    case ENCODER_MODE: {
      ret = ::BZ2_bzCompress(&stream_, action);
      break;
    }
    case DECODER_MODE: {
      ret = ::BZ2_bzDecompress(&stream_);
      break;
    }
    default: {
      return false;
    }
  }
  if (ret == BZ_STREAM_END) {
    is_end_ = true;
  }
  return (ret >= 0) || (ret == BZ_PARAM_ERROR);
}

}  // namespace nwc_toolkit
