// Copyright 2010 Susumu Yata <syata@acm.org>

#include <ctime>
#include <iomanip>
#include <iostream>

#include <nwc-toolkit/input-file.h>
#include <nwc-toolkit/output-file.h>
#include <nwc-toolkit/unicode-normalizer.h>

namespace {

class UnicodeNormalizer {
 public:
  UnicodeNormalizer()
      : form_(nwc_toolkit::UnicodeNormalizer::DEFAULT_FORM),
        handler_(nwc_toolkit::UnicodeNormalizer::DEFAULT_HANDLER) {}
  ~UnicodeNormalizer() {}

  nwc_toolkit::UnicodeNormalizer::NormalizationForm form() const {
    return form_;
  }
  nwc_toolkit::UnicodeNormalizer::IllegalInputHandler handler() const {
    return handler_;
  }

  void set_form(nwc_toolkit::UnicodeNormalizer::NormalizationForm form) {
    form_ = form;
  }
  void set_handler(
      nwc_toolkit::UnicodeNormalizer::IllegalInputHandler handler) {
    handler_ = handler;
  }

  void ParseOptions(int *argc, char *argv[]);

  bool Normalize(nwc_toolkit::InputFile *input_file,
      nwc_toolkit::OutputFile *output_file);

 private:
  enum { FLUSH_THRESHOLD = 1 << 20 };

  nwc_toolkit::UnicodeNormalizer::NormalizationForm form_;
  nwc_toolkit::UnicodeNormalizer::IllegalInputHandler handler_;

  // Disallows copy and assignment.
  UnicodeNormalizer(const UnicodeNormalizer &);
  UnicodeNormalizer &operator=(const UnicodeNormalizer &);
};

void UnicodeNormalizer::ParseOptions(int *argc, char *argv[]) {
  int new_argc = 1;
  for (int i = 1; i < *argc; ++i) {
    nwc_toolkit::String arg = argv[i];
    if (arg.Compare("--NFC", nwc_toolkit::ToLower()) == 0) {
      set_form(nwc_toolkit::UnicodeNormalizer::NFC);
    } else if (arg.Compare("--NFD", nwc_toolkit::ToLower()) == 0) {
      set_form(nwc_toolkit::UnicodeNormalizer::NFD);
    } else if (arg.Compare("--NFKC", nwc_toolkit::ToLower()) == 0) {
      set_form(nwc_toolkit::UnicodeNormalizer::NFKC);
    } else if (arg.Compare("--NFKD", nwc_toolkit::ToLower()) == 0) {
      set_form(nwc_toolkit::UnicodeNormalizer::NFKD);
    } else if (arg.Compare("--keep", nwc_toolkit::ToLower()) == 0) {
      set_handler(
          nwc_toolkit::UnicodeNormalizer::KEEP_REPLACEMENT_CHARACTERS);
    } else if (arg.Compare("--remove", nwc_toolkit::ToLower()) == 0) {
      set_handler(
          nwc_toolkit::UnicodeNormalizer::REMOVE_REPLACEMENT_CHARACTERS);
    } else {
      argv[new_argc++] = argv[i];
    }
  }
  *argc = new_argc;
}

bool UnicodeNormalizer::Normalize(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  std::time_t start_time = std::time(NULL);

  long long total_in = 0;
  long long total_out = 0;

  nwc_toolkit::String line;
  nwc_toolkit::StringBuilder src;
  nwc_toolkit::StringBuilder dest;
  while (input_file->ReadLine(&line)) {
    src.Append(line);
    if (src.length() >= FLUSH_THRESHOLD) {
      if (!nwc_toolkit::UnicodeNormalizer::Normalize(
          form_, handler_, src.str(), &dest)) {
        std::cerr << "error: failed to normalize text" << std::endl;
        return false;
      } else if (!output_file->Write(dest.str())) {
        std::cerr << "error: failed to output normalized text" << std::endl;
        return false;
      }
      total_in += src.length();
      total_out += dest.length();
      src.Clear();
      dest.Clear();
    }
  }
  if (!src.is_empty()) {
    if (!nwc_toolkit::UnicodeNormalizer::Normalize(form_, src.str(), &dest)) {
      std::cerr << "error: failed to normalize text" << std::endl;
      return false;
    } else if (!output_file->Write(dest.str())) {
      std::cerr << "error: failed to output normalized text" << std::endl;
      return false;
    }
    total_in += src.length();
    total_out += dest.length();
  }
  std::cerr << '\r' << total_in << " / " << total_out
      << " (" << (std::time(NULL) - start_time) << "sec)" << std::endl;
  return true;
}

void PrintHelp(const char *command) {
  std::cerr << "Usage: " << command << " [OPTION]... [FILE]...\n\n"
      "Options:\n"
      "  --NFC     Normalization Form C (default)\n"
      "  --NFD     Normalization Form D\n"
      "  --NFKC    Normalization Form KC\n"
      "  --NFKD    Normalization Form KD\n"
      "            C = Composition, D = Decomposition, K = Compatibility\n"
      "  --keep    keep replacement characters (default)\n"
      "  --remove  remove replacement characters\n"
      "  --output=[FILE]  output normalized text to this file\n"
      "  --help    print this help\n"
      << std::flush;
}

}  // namespace

int main(int argc, char *argv[]) {
  UnicodeNormalizer unicode_normalizer;
  unicode_normalizer.ParseOptions(&argc, argv);

  nwc_toolkit::String output_file_path;

  int new_argc = 1;
  for (int i = 1; i < argc; ++i) {
    nwc_toolkit::String arg = argv[i];
    if (arg.StartsWith("--output", nwc_toolkit::ToLower())) {
      arg = arg.SubString(8);
      if (arg.StartsWith("=")) {
        output_file_path = arg.SubString(1);
      } else if (arg.is_empty()) {
        if ((i + 1) < argc) {
          output_file_path = argv[++i];
        } else {
          PrintHelp(argv[0]);
          return -1;
        }
      } else {
        argv[new_argc++] = argv[i];
      }
    } else if (arg.Compare("--help", nwc_toolkit::ToLower()) == 0) {
      PrintHelp(argv[0]);
      return 0;
    } else {
      argv[new_argc++] = argv[i];
    }
  }
  argc = new_argc;

  nwc_toolkit::OutputFile output_file;
  std::cerr << "output: " << (output_file_path.is_empty()
      ? "standard output" : output_file_path) << std::endl;
  if (!output_file.Open(output_file_path)) {
    std::cerr << "error: failed to open output file: "
        << output_file_path << std::endl;
    return -2;
  }

  if (argc == 1) {
    nwc_toolkit::InputFile input_file;
    std::cerr << "input: standard input" << std::endl;
    if (!input_file.Open(NULL)) {
      std::cerr << "error: failed to open standard input: " << std::endl;
      return -3;
    } else if (!unicode_normalizer.Normalize(&input_file, &output_file)) {
      return -4;
    }
  }

  for (int i = 1; i < argc; ++i) {
    nwc_toolkit::String input_file_path = argv[i];
    nwc_toolkit::InputFile input_file;
    std::cerr << "input: " << (input_file_path.is_empty()
        ? "standard input" : input_file_path) << std::endl;
    if (!input_file.Open(input_file_path)) {
      std::cerr << "error: failed to open input file: "
          << input_file_path << std::endl;
      return -3;
    } else if (!unicode_normalizer.Normalize(&input_file, &output_file)) {
      return -4;
    }
  }

  return 0;
}
