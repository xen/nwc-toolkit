// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <nwc-toolkit/html-archive-entry.h>
#include <nwc-toolkit/html-document.h>
#include <nwc-toolkit/text-filter.h>
#include <nwc-toolkit/unicode-normalizer.h>

#define NWC_TOOLKIT_ERROR(fmt, ...) \
  error_at_line(-(__LINE__), errno, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

namespace {

enum InputFileFormat {
  HTML_ARCHIVE,
  SINGLE_HTML_DOCUMENT,
  DEFAULT_FORMAT = HTML_ARCHIVE
};

InputFileFormat input_file_format = DEFAULT_FORMAT;
long long max_num_entries = 0;
nwc_toolkit::UnicodeNormalizer::NormalizationForm normalization_form;
nwc_toolkit::UnicodeNormalizer::IllegalInputHandler illegal_input_handler;
bool with_unicode_normalization = false;
bool with_text_filter = false;
nwc_toolkit::String output_file_name;
bool is_help_mode = false;

void ParseOptions(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "archive", 0, NULL, 'a' },
    { "single", 0, NULL, 's' },
    { "entries", 1, NULL, 'n' },
    { "NFC", 0, NULL, 'c' },
    { "NFD", 0, NULL, 'd' },
    { "NFKC", 0, NULL, 'C' },
    { "NFKD", 0, NULL, 'D' },
    { "keep", 0, NULL, 'k' },
    { "remove", 0, NULL, 'r' },
    { "filter", 0, NULL, 'f' },
    { "output", 1, NULL, 'o' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, '\0' }
  };

  int value;
  while ((value = ::getopt_long(argc, argv,
      "asn:cdCDkrfo:h", long_options, NULL)) != -1) {
    switch (value) {
      case 'a': {
        input_file_format = HTML_ARCHIVE;
        break;
      }
      case 's': {
        input_file_format = SINGLE_HTML_DOCUMENT;
        break;
      }
      case 'n': {
        char *end_of_value;
        max_num_entries = strtoll(optarg, &end_of_value, 10);
        if (*end_of_value != '\0') {
          NWC_TOOLKIT_ERROR("invalid argument: %s", optarg);
        }
        break;
      }
      case 'c': {
        normalization_form = nwc_toolkit::UnicodeNormalizer::NFC;
        with_unicode_normalization = true;
        break;
      }
      case 'd': {
        normalization_form = nwc_toolkit::UnicodeNormalizer::NFD;
        with_unicode_normalization = true;
        break;
      }
      case 'C': {
        normalization_form = nwc_toolkit::UnicodeNormalizer::NFKC;
        with_unicode_normalization = true;
        break;
      }
      case 'D': {
        normalization_form = nwc_toolkit::UnicodeNormalizer::NFKD;
        with_unicode_normalization = true;
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
      case 'f': {
        with_text_filter = true;
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
      "  -a, --archive  extract text from HTML archives (default)\n"
      "  -s, --single   extract text from HTML documents\n"
      "  -n, --entries=[N]    "
      "limit the maximum number of entries to N (default: 0)\n"
      "  -c, --NFC      Normalization Form C (default)\n"
      "  -d, --NFD      Normalization Form D\n"
      "  -C, --NFKC     Normalization Form KC\n"
      "  -D, --NFKD     Normalization Form KD\n"
      "                 C = Composition, D = Decomposition, K = Compatibility\n"
      "  -k, --keep     keep replacement characters (default)\n"
      "  -r, --remove   remove replacement characters\n"
      "  -f, --filter   apply text filter\n"
      "  -o, --output=[FILE]  write result to FILE (default: stdout)\n"
      "  -h, --help     print this help\n"
      << std::flush;
}

void ExtractTextFromHtmlArchvie(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  std::time_t start_time = std::time(NULL);

  nwc_toolkit::HtmlDocument document;
  nwc_toolkit::StringBuilder text;
  nwc_toolkit::StringBuilder normalized_text;
  nwc_toolkit::StringBuilder filtered_text;

  long long num_entries = 0;
  long long status_error_count = 0;
  long long parse_error_count = 0;
  nwc_toolkit::HtmlArchiveEntry entry;
  while (entry.Read(input_file)) {
    nwc_toolkit::StringBuilder *temp = &text;
    text.Clear();

    if (entry.status_code() != 200) {
      ++status_error_count;
    } else if (!document.Parse(entry)) {
      ++parse_error_count;
    } else {
      document.ExtractText(&text);

      if (with_unicode_normalization) {
        normalized_text.Clear();
        if (!nwc_toolkit::UnicodeNormalizer::Normalize(normalization_form,
            illegal_input_handler, temp->str(), &normalized_text)) {
          ++parse_error_count;
        }
        temp = &normalized_text;
      }

      if (with_text_filter) {
        filtered_text.Clear();
        nwc_toolkit::TextFilter::Filter(temp->str(), &filtered_text);
        temp = &filtered_text;
      }
    }
    temp->Append('\n');
    output_file->Write(temp->str());

    if (++num_entries == max_num_entries) {
      break;
    }

    if ((num_entries % 1000) == 0) {
      std::cerr << '\r' << status_error_count << " ("
          << std::fixed << std::setw(5) << std::setprecision(2)
          << (100.0 * status_error_count / num_entries)
          << "%) / " << parse_error_count << " ("
          << std::fixed << std::setw(5) << std::setprecision(2)
          << (100.0 * parse_error_count / num_entries)
          << "%) / " << num_entries
          << " (" << (std::time(NULL) - start_time) << "sec)";
    }
  }
  std::cerr << '\r' << status_error_count << " ("
      << std::fixed << std::setw(5) << std::setprecision(2)
      << (100.0 * status_error_count / num_entries)
      << "%) / " << parse_error_count << " ("
      << std::fixed << std::setw(5) << std::setprecision(2)
      << (100.0 * parse_error_count / num_entries)
      << "%) / " << num_entries
      << " (" << (std::time(NULL) - start_time) << "sec)" << std::endl;
}

void ExtractTextFromSingleHtmlDocument(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  nwc_toolkit::String line;
  nwc_toolkit::StringBuilder body;
  while (input_file->ReadLine(&line)) {
    body.Append(line);
  }

  nwc_toolkit::HtmlDocument document;
  if (document.Parse(body.str())) {
    nwc_toolkit::StringBuilder text;
    document.ExtractText(&text);
    nwc_toolkit::StringBuilder *temp = &text;

    nwc_toolkit::StringBuilder normalized_text;
    if (with_unicode_normalization) {
      if (!nwc_toolkit::UnicodeNormalizer::Normalize(normalization_form,
          illegal_input_handler, temp->str(), &normalized_text)) {
      }
      temp = &normalized_text;
    }

    nwc_toolkit::StringBuilder filtered_text;
    if (with_text_filter) {
      nwc_toolkit::TextFilter::Filter(temp->str(), &filtered_text);
      temp = &filtered_text;
    }

    temp->Append('\n');
    if (!output_file->Write(temp->str())) {
      NWC_TOOLKIT_ERROR("failed to write result");
    }
  }
}

void ExtractText(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  switch (input_file_format) {
    case HTML_ARCHIVE: {
      ExtractTextFromHtmlArchvie(input_file, output_file);
      break;
    }
    case SINGLE_HTML_DOCUMENT: {
      ExtractTextFromSingleHtmlDocument(input_file, output_file);
      break;
    }
    default: {
      NWC_TOOLKIT_ERROR("invalid input file format");
    }
  }
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
    ExtractText(&input_file, &output_file);
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
    ExtractText(&input_file, &output_file);
  }

  return 0;
}
