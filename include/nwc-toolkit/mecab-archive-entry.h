// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_MECAB_ARCHIVE_ENTRY_H_
#define NWC_TOOLKIT_MECAB_ARCHIVE_ENTRY_H_

#include <vector>

#include "./input-file.h"
#include "./output-file.h"
#include "./string-pool.h"

namespace nwc_toolkit {

class MecabArchiveEntry {
 public:
  MecabArchiveEntry() : word_pool_(), words_() {}
  ~MecabArchiveEntry() {
    Clear();
  }

  std::size_t num_words() const { return words_.size(); }
  const String &word(std::size_t id) const { return words_[id]; }

  void Clear() {
    words_.clear();
    word_pool_.Clear();
  }

  bool Read(InputFile *file) {
    Clear();
    String line;
    while (file->ReadLine(&line)) {
      if (!line.EndsWith("\n")) {
        return false;
      }
      String word = line.SubString(0, line.length() - 1);
      if (word == "EOS") {
        if (words_.empty() || (words_.back() == "EOS")) {
          return true;
        }
      }
      words_.push_back(word_pool_.Append(word));
    }
    return false;
  }

  bool Write(OutputFile *file) const {
    for (std::size_t i = 0; i < words_.size(); ++i) {
      if (!file->Write(words_[i]) || !file->Write("\n")) {
        return false;
      }
    }
    return file->Write("EOS\n");
  }

 private:
  StringPool word_pool_;
  std::vector<String> words_;

  // Disallows copy and assignment.
  MecabArchiveEntry(const MecabArchiveEntry &);
  MecabArchiveEntry &operator=(const MecabArchiveEntry &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_MECAB_ARCHIVE_ENTRY_H_
