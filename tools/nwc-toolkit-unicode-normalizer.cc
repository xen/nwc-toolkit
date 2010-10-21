// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <ctime>
#include <iomanip>
#include <iostream>

#include <nwc-toolkit/input-file.h>
#include <nwc-toolkit/output-file.h>
#include <nwc-toolkit/unicode-normalizer.h>

#define NWC_TOOLKIT_ERROR(fmt, ...) \
  error_at_line(-(__LINE__), errno, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

namespace {

enum { FLUSH_THRESHOLD = 1 << 20 };

nwc_toolkit::UnicodeNormalizer::NormalizationForm normalization_form;
nwc_toolkit::UnicodeNormalizer::IllegalInputHandler illegal_input_handler;
nwc_toolkit::String output_file_name;
bool is_help_mode = false;

void ParseOptions(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "NFC", 0, NULL, 'c' },
    { "NFD", 0, NULL, 'd' },
    { "NFKC", 0, NULL, 'C' },
    { "NFKD", 0, NULL, 'D' },
    { "keep", 0, NULL, 'k' },
    { "remove", 0, NULL, 'r' },
    { "output", 1, NULL, 'o' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, '\0' }
  };

  int value;
  while ((value = ::getopt_long(argc, argv,
      "cdCDkro:h", long_options, NULL)) != -1) {
    switch (value) {
      case 'c': {
        normalization_form = nwc_toolkit::UnicodeNormalizer::NFC;
        break;
      }
      case 'd': {
        normalization_form = nwc_toolkit::UnicodeNormalizer::NFD;
        break;
      }
      case 'C': {
        normalization_form = nwc_toolkit::UnicodeNormalizer::NFKC;
        break;
      }
      case 'D': {
        normalization_form = nwc_toolkit::UnicodeNormalizer::NFKD;
        break;
      }
      case 'k': {
        illegal_input_handler =
            nwc_toolkit::UnicodeNormalizer::KEEP_REPLACEMENT_CHARACTERS;
        break;
      }
      case 'r': {
        illegal_input_handler =
            nwc_toolkit::UnicodeNormalizer::REMOVE_REPLACEMENT_CHARACTERS;
        break;
      }
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
      "  -c, --NFC     Normalization Form C (default)\n"
      "  -d, --NFD     Normalization Form D\n"
      "  -C, --NFKC    Normalization Form KC\n"
      "  -D, --NFKD    Normalization Form KD\n"
      "            C = Composition, D = Decomposition, K = Compatibility\n"
      "  -k, --keep    keep replacement characters (default)\n"
      "  -r, --remove  remove replacement characters\n"
      "  -o, --output=[FILE]  write result to FILE (default: stdout)\n"
      "  -h, --help    print this help\n"
      << std::flush;
}

void Normalize(nwc_toolkit::InputFile *input_file,
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
          normalization_form, illegal_input_handler, src.str(), &dest)) {
        NWC_TOOLKIT_ERROR("failed to normalize text");
      } else if (!output_file->Write(dest.str())) {
        NWC_TOOLKIT_ERROR("failed to write result");
      }
      total_in += src.length();
      total_out += dest.length();
      src.Clear();
      dest.Clear();
    }
  }
  if (!src.is_empty()) {
    if (!nwc_toolkit::UnicodeNormalizer::Normalize(
        normalization_form, src.str(), &dest)) {
      NWC_TOOLKIT_ERROR("failed to normalize text");
    } else if (!output_file->Write(dest.str())) {
      NWC_TOOLKIT_ERROR("failed to write result");
    }
    total_in += src.length();
    total_out += dest.length();
  }
  std::cerr << '\r' << total_in << " / " << total_out
      << " (" << (std::time(NULL) - start_time) << "sec)" << std::endl;
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
    Normalize(&input_file, &output_file);
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
    Normalize(&input_file, &output_file);
  }

  return 0;
}
