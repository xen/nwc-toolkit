// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/html-archive-entry.h>

#include <cstdlib>
#include <limits>

namespace nwc_toolkit {

bool HtmlArchiveEntry::Read(InputFile *file) {
  String url_line;
  if (!file->ReadLine(&url_line)) {
    return false;
  }
  set_url(url_line.StripRight());

  int status_code = 0;
  if (!ReadInt(file, &status_code)) {
    return false;
  }
  set_status_code(status_code);

  int header_length = 0;
  if (!ReadInt(file, &header_length)) {
    return false;
  }
  String header;
  if (!file->Read(header_length, &header)) {
    return false;
  }
  set_header(header);

  int body_length;
  if (!ReadInt(file, &body_length)) {
    return false;
  }
  String body;
  if (!file->Read(body_length, &body)) {
    return false;
  }
  set_body(body);
  return true;
}

void HtmlArchiveEntry::Clear() {
  url_.Clear();
  status_code_ = 0;
  header_.Clear();
  body_.Clear();
}

bool HtmlArchiveEntry::Write(OutputFile *file) const {
  if (!file->Write(url()) || !file->Write("\n")) {
    return false;
  }

  if (!WriteInt(status_code(), file)) {
    return false;
  }

  if (!WriteInt(static_cast<int>(header_.length()), file) ||
      !file->Write(header())) {
    return false;
  }

  if (!WriteInt(static_cast<int>(body_.length()), file) ||
      !file->Write(body())) {
    return false;
  }
  return true;
}

bool HtmlArchiveEntry::ReadInt(InputFile *file, int *value) {
  String line;
  if (!file->ReadLine(&line)) {
    return false;
  }
  char *value_end = NULL;
  long long_value = std::strtol(line.ptr(), &value_end, 10);
  if (*value_end != '\n') {
    return false;
  }
  if ((long_value < 0) || (long_value > std::numeric_limits<int>::max())) {
    return false;
  }
  *value = static_cast<int>(long_value);
  return true;
}

bool HtmlArchiveEntry::WriteInt(int value, OutputFile *file) {
  if (value < 0) {
    return false;
  }
  char buf[16];
  std::size_t pos = sizeof(buf);
  buf[--pos] = '\n';
  do {
    buf[--pos] = '0' + (value % 10);
    value /= 10;
  } while (value > 0);
  return file->Write(String(buf + pos, sizeof(buf) - pos));
}

}  // namespace nwc_toolkit
