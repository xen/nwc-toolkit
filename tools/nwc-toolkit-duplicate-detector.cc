// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <nwc-toolkit/input-file.h>
#include <nwc-toolkit/output-file.h>

#define NWC_TOOLKIT_ERROR(fmt, ...) \
  error_at_line(-(__LINE__), errno, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

namespace {

nwc_toolkit::String output_file_name;
bool is_help_mode = false;

void ParseOptions(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "output", 1, NULL, 'o' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, '\0' }
  };

  int value;
  while ((value = ::getopt_long(argc, argv,
      "o:h", long_options, NULL)) != -1) {
    switch (value) {
      case 'o': {
        output_file_name = optarg;
        break;
      }
      case 'h': {
        is_help_mode = true;
        break;
      }
      default: {
        NWC_TOOLKIT_ERROR("invalid option");
      }
    }
  }
}

void PrintHelp(const char *command) {
  std::cerr << "Usage: " << command << " [OPTION]... [FILE]...\n\n"
      "Options:\n"
      "  -o, --output=[FILE]  write result to FILE (default: stdout)\n"
      "  -h, --help    print this help\n"
      << std::flush;
}

void Detect(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  std::time_t start_time = std::time(NULL);

  long long input_count = 0;
  long long output_count = 0;

  nwc_toolkit::String line;
  nwc_toolkit::StringBuilder last_hash;
  while (input_file->ReadLine(&line)) {
    nwc_toolkit::String delim = line.FindFirstOf('\t');
    if (delim.is_empty()) {
      NWC_TOOLKIT_ERROR("invalid format: no delimitor: %.*s",
          static_cast<int>(line.length()), line.ptr());
    }

    nwc_toolkit::String hash(line.begin(), delim.begin());
    if (hash.length() != 40) {
      NWC_TOOLKIT_ERROR("invalid format: 1st field must be SHA-1 hash: %.*s",
          static_cast<int>(line.length()), line.ptr());
    }

    if (hash == last_hash.str()) {
      nwc_toolkit::String url(delim.end(), line.end());
      if (!output_file->Write(url)) {
        NWC_TOOLKIT_ERROR("failed to write result");
      }
      ++output_count;
    } else if (hash < last_hash.str()) {
      NWC_TOOLKIT_ERROR("invalid order: input must be sorted");
    } else {
      last_hash = hash;
    }

    if ((++input_count % 1000) == 0) {
      std::cerr << '\r' << output_count << " / " << input_count << " ("
          << std::fixed << std::setw(5) << std::setprecision(2)
          << ((input_count != 0) ? (100.0 * output_count / input_count) : 0.0)
          << "%) (" << (std::time(NULL) - start_time) << "sec)";
    }
  }
  std::cerr << '\r' << output_count << " / " << input_count << " ("
      << std::fixed << std::setw(5) << std::setprecision(2)
      << ((input_count != 0) ? (100.0 * output_count / input_count) : 0.0)
      << "%) (" << (std::time(NULL) - start_time) << "sec)" << std::endl;
}

}  // namespace

int main(int argc, char *argv[]) {
  ParseOptions(argc, argv);
  if (is_help_mode) {
    PrintHelp(argv[0]);
    return 0;
  }

  nwc_toolkit::OutputFile output_file;
  std::cerr << "output: " << (output_file_name.is_empty()
      ? "(standard output)" : output_file_name) << std::endl;
  if (!output_file.Open(output_file_name)) {
    NWC_TOOLKIT_ERROR("failed to open output file: %s",
        output_file_name.ptr());
  }

  if (optind == argc) {
    nwc_toolkit::InputFile input_file;
    std::cerr << "input: (standard input)" << std::endl;
    if (!input_file.Open(NULL)) {
      NWC_TOOLKIT_ERROR("failed to open standard input");
    }
    Detect(&input_file, &output_file);
  }

  for (int i = optind; i < argc; ++i) {
    nwc_toolkit::String input_file_name = argv[i];
    std::cerr << "input: " << (input_file_name.is_empty()
        ? "(standard input)" : input_file_name) << std::endl;
    nwc_toolkit::InputFile input_file;
    if (!input_file.Open(input_file_name)) {
      NWC_TOOLKIT_ERROR("failed to open input file: %s",
          input_file_name.ptr());
    }
    Detect(&input_file, &output_file);
  }

  return 0;
}
