// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CODER_H_
#define NWC_TOOLKIT_CODER_H_

#include <cstddef>

namespace nwc_toolkit {

class Coder {
 public:
  enum Mode { NO_MODE, ENCODER_MODE, DECODER_MODE };
  enum { DEFAULT_PRESET = 6, EXTREME_PRESET_FLAG = 1 << 8 };

  Coder() {}
  virtual ~Coder() {}

  virtual bool OpenEncoder(int preset = DEFAULT_PRESET) = 0;
  virtual bool OpenDecoder() = 0;
  virtual bool Close() = 0;

  virtual bool Code() = 0;
  virtual bool Finish() = 0;

  virtual Mode mode() const = 0;
  virtual bool is_open() const = 0;
  virtual bool is_end() const = 0;

  virtual const void *next_in() const = 0;
  virtual std::size_t avail_in() const = 0;
  virtual unsigned long long total_in() const = 0;
  virtual void *next_out() const = 0;
  virtual std::size_t avail_out() const = 0;
  virtual unsigned long long total_out() const = 0;

  virtual void set_next_in(const void *next_in) = 0;
  virtual void set_avail_in(std::size_t avail_in) = 0;
  virtual void set_next_out(void *next_out) = 0;
  virtual void set_avail_out(std::size_t avail_out) = 0;

 private:
  // Non-copyable.
  Coder(const Coder &);
  Coder &operator=(const Coder &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CODER_H_
