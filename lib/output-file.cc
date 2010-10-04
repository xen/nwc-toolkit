// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/output-file.h>

#include <cstring>

namespace nwc_toolkit {

bool OutputFile::Open(const String &path, std::size_t io_buf_size,
    int coder_preset) {
  if (is_open()) {
    return false;
  }

  StringBuilder sz_path;
  sz_path.Append(path).Append();

  if (sz_path.str().EndsWith(".gz", ToLower())) {
    coder_.reset(new GzipCoder);
  } else if (sz_path.str().EndsWith(".bz2", ToLower())) {
    coder_.reset(new Bzip2Coder);
  } else if (sz_path.str().EndsWith(".xz", ToLower())) {
    coder_.reset(new XzCoder);
  }

  if (io_buf_size == 0) {
    io_buf_size = DEFAULT_IO_BUF_SIZE;
  }
  io_buf_.Resize(io_buf_size);

  if (coder_ != NULL) {
    if (!coder_->OpenEncoder(coder_preset)) {
      return false;
    }
    coder_->set_next_out(io_buf_.buf());
    coder_->set_avail_out(io_buf_.size());
  }

  if (sz_path.is_empty()) {
    file_ = ::stdout;
  } else {
    file_ = std::fopen(sz_path.ptr(), "wb");
    if (file_ == NULL) {
      return false;
    }

    if (coder_ == NULL) {
      std::setvbuf(file_, io_buf_.buf(), _IOFBF, io_buf_.size());
    } else {
      std::setvbuf(file_, NULL, _IONBF, 0);
    }
  }

  return true;
}

bool OutputFile::Close() {
  if (!is_open()) {
    return false;
  }

  bool flush_ok = true;
  if (coder_ != NULL) {
    while (!coder_->is_end())
    {
      if (!coder_->Finish()) {
        flush_ok = false;
        break;
      }
      std::size_t avail = io_buf_.size() - coder_->avail_out();
      if (std::fwrite(io_buf_.ptr(), 1, avail, file_) != avail) {
        flush_ok = false;
        break;
      }
      coder_->set_next_out(io_buf_.buf());
      coder_->set_avail_out(io_buf_.size());
    }
  }

  if (file_ != ::stdout) {
    if (std::fclose(file_) != 0) {
      flush_ok = false;
    }
  }

  file_ = NULL;
  coder_.reset();
  io_buf_.Clear();
  return flush_ok;
}

bool OutputFile::Write(const String &str) {
  if (!is_open()) {
    return false;
  }

  if (coder_ == NULL) {
    return std::fwrite(str.ptr(), 1, str.length(), file_) == str.length();
  }

  coder_->set_next_in(str.ptr());
  coder_->set_avail_in(str.length());
  while (coder_->avail_in() > 0) {
    if (!coder_->Code()) {
      return false;
    } else if (coder_->avail_out() == 0) {
      std::size_t size_written = std::fwrite(
          io_buf_.ptr(), 1, io_buf_.size(), file_);
      if (size_written != io_buf_.size()) {
        return false;
      }
      coder_->set_next_out(io_buf_.buf());
      coder_->set_avail_out(io_buf_.size());
    }
  }

  return true;
}

}  // namespace nwc_toolkit
