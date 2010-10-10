// Copyright 2010 Susumu Yata <syata@acm.org>

#include <ctime>
#include <iomanip>
#include <iostream>

#include <nwc-toolkit/input-file.h>
#include <nwc-toolkit/output-file.h>
#include <nwc-toolkit/text-filter.h>

namespace {

class TextFilter {
 public:
  TextFilter() {}
  ~TextFilter() {}

  void ParseOptions(int *argc, char *argv[]) {
  }

  bool Filter(nwc_toolkit::InputFile *input_file,
      nwc_toolkit::OutputFile *output_file);

 private:
  enum { FLUSH_THRESHOLD = 1 << 20 };

  // Disallows copy and assignment.
  TextFilter(const TextFilter &);
  TextFilter &operator=(const TextFilter &);
};

bool TextFilter::Filter(nwc_toolkit::InputFile *input_file,
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
      nwc_toolkit::TextFilter::Filter(src.str(), &dest);
      if (!output_file->Write(dest.str())) {
        std::cerr << "error: failed to output filtered text" << std::endl;
        return false;
      }
      total_in += src.length();
      total_out += dest.length();
      src.Clear();
      dest.Clear();
    }
  }
  if (!src.is_empty()) {
    nwc_toolkit::TextFilter::Filter(src.str(), &dest);
    if (!output_file->Write(dest.str())) {
      std::cerr << "error: failed to output filtered text" << std::endl;
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
      "  --output=[FILE]  output filtered text to this file\n"
      "  --help  print this help\n"
      << std::flush;
}

}  // namespace

int main(int argc, char *argv[]) {
  TextFilter text_filter;
  text_filter.ParseOptions(&argc, argv);

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
    } else if (!text_filter.Filter(&input_file, &output_file)) {
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
    } else if (!text_filter.Filter(&input_file, &output_file)) {
      return -4;
    }
  }

  return 0;
}
