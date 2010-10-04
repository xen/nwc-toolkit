// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/xz-coder.h>

namespace nwc_toolkit {

bool XzCoder::OpenEncoder(int preset) {
  if (is_open()) {
    return false;
  }
  int xz_preset = preset;
  switch (preset) {
    case DEFAULT_PRESET: {
      xz_preset = 6;
      break;
    }
    case BEST_SPEED_PRESET: {
      xz_preset = 0;
      break;
    }
    case BEST_COMPRESSION_PRESET: {
      xz_preset = 9 | LZMA_PRESET_EXTREME;
      break;
    }
    default: {
      xz_preset = preset & ~EXTREME_PRESET_FLAG;
      if ((xz_preset < 0) || (xz_preset > 9)) {
        return false;
      } else if ((preset & EXTREME_PRESET_FLAG) == EXTREME_PRESET_FLAG) {
        xz_preset |= LZMA_PRESET_EXTREME;
      }
    }
  }
  ::lzma_ret ret = ::lzma_easy_encoder(&stream_, xz_preset, LZMA_CHECK_CRC64);
  if (ret != LZMA_OK) {
    return false;
  }
  mode_ = ENCODER_MODE;
  return true;
}

bool XzCoder::OpenDecoder() {
  if (is_open()) {
    return false;
  }
  ::lzma_ret ret = ::lzma_stream_decoder(&stream_, 128 << 20, 0);
  if (ret != LZMA_OK) {
    return false;
  }
  mode_ = DECODER_MODE;
  return true;
}

bool XzCoder::Close() {
  if (!is_open()) {
    return false;
  }
  ::lzma_end(&stream_);
  InitStream();
  mode_ = NO_MODE;
  is_end_ = false;
  return true;
}

bool XzCoder::Code(::lzma_action action) {
  if (!is_open() || is_end()) {
    return false;
  }
  ::lzma_ret ret = ::lzma_code(&stream_, action);
  if (ret == LZMA_STREAM_END) {
    is_end_ = true;
    return true;
  }
  return (ret == LZMA_OK) || (ret == LZMA_BUF_ERROR);
}

}  // namespace nwc_toolkit
