// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_TEXT_ARCHIVE_ENTRY_H_
#define NWC_TOOLKIT_TEXT_ARCHIVE_ENTRY_H_

#include <vector>

#include "./input-file.h"
#include "./output-file.h"
#include "./string-pool.h"

namespace nwc_toolkit {

class TextArchiveEntry {
 public:
  TextArchiveEntry() : line_pool_(), lines_() {}
  ~TextArchiveEntry() {
    Clear();
  }

  std::size_t num_lines() const { return lines_.size(); }
  const String &line(std::size_t id) const { return lines_[id]; }

  void Clear() {
    lines_.clear();
    line_pool_.Clear();
  }

  bool Read(InputFile *file) {
    Clear();
    String line;
    while (file->ReadLine(&line)) {
      if (!line.EndsWith("\n")) {
        return false;
      }
      line = line.SubString(0, line.length() - 1);
      if (line.is_empty()) {
        return true;
      }
      lines_.push_back(line_pool_.Append(line));
    }
    return false;
  }

  bool Write(OutputFile *file) const {
    for (std::size_t i = 0; i < lines_.size(); ++i) {
      if (!file->Write(lines_[i]) || !file->Write("\n")) {
        return false;
      }
    }
    return file->Write("\n");
  }

 private:
  StringPool line_pool_;
  std::vector<String> lines_;

  // Disallows copy and assignment.
  TextArchiveEntry(const TextArchiveEntry &);
  TextArchiveEntry &operator=(const TextArchiveEntry &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_TEXT_ARCHIVE_ENTRY_H_
