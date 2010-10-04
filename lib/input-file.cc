// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/input-file.h>

#include <cstring>

namespace nwc_toolkit {

bool InputFile::Open(const String &path, std::size_t io_buf_size,
    std::size_t coder_buf_size) {
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
    if (!coder_->OpenDecoder()) {
      return false;
    }

    if (coder_buf_size == 0) {
      coder_buf_size = DEFAULT_CODER_BUF_SIZE;
    }
    coder_buf_.Resize(coder_buf_size);
  }

  if (sz_path.is_empty()) {
    file_ = ::stdin;
  } else {
    file_ = std::fopen(sz_path.ptr(), "rb");
    if (file_ == NULL) {
      return false;
    }
    std::setvbuf(file_, NULL, _IONBF, 0);
  }
  return true;
}

bool InputFile::Close() {
  if (!is_open()) {
    return false;
  }

  if (file_ != ::stdin) {
    std::fclose(file_);
  }

  file_ = NULL;
  coder_.reset();
  io_buf_.Clear();
  coder_buf_.Clear();
  next_ = NULL;
  avail_ = 0;
  return true;
}

bool InputFile::Read(std::size_t size, String *data) {
  if (!is_open()) {
    return false;
  }

  data->Clear();
  if (size > avail_) {
    ShiftToFront(size);
    if (!FillBuf()) {
      return false;
    }
  }
  data->Assign(next_, size);
  next_ += size;
  avail_ -= size;
  return true;
}

bool InputFile::ReadLine(char delim, String *line) {
  if (!is_open()) {
    return false;
  }

  line->Clear();
  std::size_t pos = 0;
  do {
    for ( ; pos < avail_; ++pos) {
      if (next_[pos] == delim) {
        line->Assign(next_, pos + 1);
        next_ += pos + 1;
        avail_ -= pos + 1;
        return true;
      }
    }
    ShiftToFront(pos);
  } while (FillBuf());
  if (pos == 0) {
    return false;
  }
  line->Assign(next_, pos);
  next_ += pos;
  avail_ -= pos;
  return true;
}

void InputFile::ShiftToFront(std::size_t request_size) {
  StringBuilder *front_buf = (coder_ != NULL) ? &coder_buf_ : &io_buf_;
  if (next_ != front_buf->ptr()) {
    std::memmove(front_buf->buf(), next_, avail_);
    next_ = front_buf->ptr();
  }
  if (request_size > front_buf->size() / 2) {
    front_buf->Resize(request_size * 2);
    next_ = front_buf->ptr();
  }
}

bool InputFile::FillBuf() {
  if (coder_ == NULL) {
    std::size_t size_read = std::fread(io_buf_.buf() + avail_,
        1, io_buf_.size() - avail_, file_);
    avail_ += size_read;
    return size_read > 0;
  }

  coder_->set_next_out(coder_buf_.buf() + avail_);
  coder_->set_avail_out(coder_buf_.size() - avail_);
  while (coder_->Code()) {
    if (coder_->avail_out() == 0) {
      break;
    } else if (coder_->avail_in() == 0) {
      std::size_t size_read = std::fread(io_buf_.buf(),
          1, io_buf_.size(), file_);
      if (size_read == 0) {
        break;
      }
      coder_->set_next_in(io_buf_.ptr());
      coder_->set_avail_in(size_read);
    } else {
      return false;
    }
  }
  std::size_t new_avail = coder_buf_.size() - coder_->avail_out();
  if (new_avail == avail_) {
    return false;
  }
  avail_ = new_avail;
  return true;
}

}  // namespace nwc_toolkit
