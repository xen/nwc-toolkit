// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include <nwc-toolkit/ngram-counter.h>

#define NWC_TOOLKIT_ERROR(fmt, ...) \
  error_at_line(-(__LINE__), errno, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

namespace {

enum {
  MIN_MAX_NGRAM_LENGTH =
      nwc_toolkit::NgramCounter::MIN_MAX_NGRAM_LENGTH,
  MAX_MAX_NGRAM_LENGTH = 32,
  DEFAULT_MAX_NGRAM_LENGTH =
      nwc_toolkit::NgramCounter::DEFAULT_MAX_NGRAM_LENGTH
};

enum {
  MIN_MEMORY_LIMIT =
      nwc_toolkit::NgramCounter::MIN_MEMORY_LIMIT >> 20,
  MAX_MEMORY_LIMIT = 256 * 1024,
  DEFAULT_MEMORY_LIMIT =
      nwc_toolkit::NgramCounter::DEFAULT_MEMORY_LIMIT >> 20
};

enum {
  MIN_MAX_FILE_ID = 0,
  MAX_MAX_FILE_ID = 9999,
  DEFAULT_MAX_FILE_ID = 99
};

const char * const DEFAULT_OUTPUT_FILE_PREFIX = "ngms-%Y%m%d-%H%M%S";

int max_file_id = DEFAULT_MAX_FILE_ID;
nwc_toolkit::String output_file_prefix;
nwc_toolkit::String output_file_extension = "gz";
bool is_help_mode = false;

nwc_toolkit::NgramCounter ngram_counter;
int file_id = 0;
std::time_t start_time = std::time(NULL);

int ParseIntegerValue(const char *arg, long min_value, long max_value) {
  char *end_of_value;
  long value = strtol(optarg, &end_of_value, 10);
  if (*end_of_value != '\0' || (value < min_value) || (value > max_value)) {
    return -1;
  }
  return static_cast<int>(value);
}

void ParseOptions(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "tokens", 1, NULL, 'n' },
    { "memory", 1, NULL, 'l' },
    { "wakati", 0, NULL, 'w' },
    { "mecab", 0, NULL, 'm' },
    { "chasen", 0, NULL, 'c' },
    { "boundary", 0, NULL, 'b' },
    { "sort", 0, NULL, 's' },
    { "prefix", 1, NULL, 'p' },
    { "extension", 1, NULL, 'e' },
    { "files", 1, NULL, 'f' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, '\0' }
  };

  int max_ngram_length = DEFAULT_MAX_NGRAM_LENGTH;
  std::size_t memory_limit = DEFAULT_MEMORY_LIMIT << 20;

  int value;
  while ((value = ::getopt_long(argc, argv,
      "n:l:wmcbsp:s:f:h", long_options, NULL)) != -1) {
    switch (value) {
      case 'n': {
        max_ngram_length = ParseIntegerValue(optarg,
            MIN_MAX_NGRAM_LENGTH, MAX_MAX_NGRAM_LENGTH);
        if (max_ngram_length < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
        break;
      }
      case 'l': {
        memory_limit = ParseIntegerValue(optarg,
            MIN_MEMORY_LIMIT, MAX_MEMORY_LIMIT);
        if (memory_limit < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
        if (memory_limit > (std::numeric_limits<std::size_t>::max() >> 20)) {
          NWC_TOOLKIT_ERROR("too large memory usage: `%c', %s", value, optarg);
        }
        memory_limit <<= 20;
        break;
      }
      case 'w': {
        ngram_counter.set_input_format(
            nwc_toolkit::NgramCounter::WAKATI_FORMAT);
        break;
      }
      case 'm': {
        ngram_counter.set_input_format(
            nwc_toolkit::NgramCounter::MECAB_FORMAT);
        break;
      }
      case 'c': {
        ngram_counter.set_input_format(
            nwc_toolkit::NgramCounter::CHASEN_FORMAT);
        break;
      }
      case 'b': {
        ngram_counter.set_with_boundary_count(true);
        break;
      }
      case 's': {
        ngram_counter.set_with_result_sort(true);
        break;
      }
      case 'p': {
        output_file_prefix = optarg;
        break;
      }
      case 'e': {
        output_file_extension = optarg;
        break;
      }
      case 'f': {
        max_file_id = ParseIntegerValue(optarg,
            MIN_MAX_FILE_ID, MAX_MAX_FILE_ID);
        if (max_file_id < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
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
  ngram_counter.Reset(max_ngram_length, memory_limit);
}

void PrintHelp(const char *command) {
  std::cerr << "Usage: " << command << " [OPTION]... [FILE]...\n\n"
      "Options:\n"
      "  -n, --tokens=[N: " << MIN_MAX_NGRAM_LENGTH
      << '-' << MAX_MAX_NGRAM_LENGTH << "]\n"
      "                  limit the length of n-grams to N (default: 5)\n"
      "  -l, --memory=[N: " << MIN_MEMORY_LIMIT
      << '-' << MAX_MEMORY_LIMIT << "]\n"
      "                  limit the memory usage to N MiB (default: 1024)\n"
      "  -w, --wakati    input wakati-formatted text (default)\n"
      "  -m, --mecab     input mecab-formatted text\n"
      "  -c, --chasen    input chasen-formatted text\n"
      "  -b, --boundary  count sentence boundaries as <S> and </S>\n"
      "  -s, --sort      sort result\n"
      "  -p, --prefix=[S]     set the prefix of output files\n"
      "                       (default: "<< DEFAULT_OUTPUT_FILE_PREFIX << ")\n"
      "  -e, --extension=[S]  set the extension of output files (default: gz)\n"
      "                       gz, bz2, or xz forces compression of result\n"
      "  -f, --files=[N: " << MIN_MAX_FILE_ID
      << '-' << MAX_MAX_FILE_ID << "]\n"
      "                  limit the number of output files to N (default: 100)\n"
      "  -h, --help      print this help\n"
      << std::flush;
}

void PrintProgress() {
  double num_tokens_per_sentence = 0.0;
  if (ngram_counter.sentence_count() != 0) {
    num_tokens_per_sentence = 1.0 * ngram_counter.token_count()
        / ngram_counter.sentence_count();
  }
  std::cerr << '\r' << "sentences: " << ngram_counter.sentence_count()
      << ", tokens: " << ngram_counter.token_count()
      << " (x" << std::fixed << std::setprecision(2) << num_tokens_per_sentence
      << ") (" << (std::time(NULL) - start_time) << "sec)";
}

// This function must not be called more than once.
void GenerateDefaultOutputFilePrefix() {
  static char static_buf[32];
  std::time_t current_time = std::time(NULL);
  std::strftime(static_buf, sizeof(static_buf), DEFAULT_OUTPUT_FILE_PREFIX,
      std::localtime(&current_time));
  output_file_prefix = static_buf;
}

void OpenNextOutputFile(nwc_toolkit::OutputFile *output_file) {
  if (file_id >= max_file_id) {
    NWC_TOOLKIT_ERROR("too many output files: %d", file_id + 1);
  }
  std::stringstream stream;
  stream << output_file_prefix;
  stream << '.' << std::setw(4) << std::setfill('0') << file_id;
  if (!output_file_extension.is_empty()) {
    stream << '.' << output_file_extension;
  }
  std::string output_file_name = stream.str();
  std::cerr << "output: " << output_file_name << std::endl;
  if (!output_file->Open(output_file_name.c_str())) {
    NWC_TOOLKIT_ERROR("failed to open file: %s", output_file_name.c_str());
  }
  ++file_id;
}

void FlushNgrams() {
  PrintProgress();
  std::cerr << std::endl;

  nwc_toolkit::OutputFile output_file;
  OpenNextOutputFile(&output_file);
  if (!ngram_counter.Flush(&output_file)) {
    NWC_TOOLKIT_ERROR("failed to flush n-grams");
  }
}

void CountNgrams(nwc_toolkit::InputFile *input_file) {
  while (ngram_counter.Count(input_file)) {
    if (ngram_counter.NeedsFlush()) {
      FlushNgrams();
    }
    if ((ngram_counter.sentence_count() % 10000) == 0) {
      PrintProgress();
    }
  }
  if (!ngram_counter.is_empty()) {
    FlushNgrams();
  }
}

}  // namespace

int main(int argc, char *argv[]) {
  ParseOptions(argc, argv);
  if (is_help_mode) {
    PrintHelp(argv[0]);
    return 0;
  }

  if (output_file_prefix.is_empty()) {
    GenerateDefaultOutputFilePrefix();
  }

  if (optind == argc) {
    nwc_toolkit::InputFile input_file;
    std::cerr << "input: (standard input)" << std::endl;
    if (!input_file.Open(NULL)) {
      NWC_TOOLKIT_ERROR("failed to open standard input");
    }
    CountNgrams(&input_file);
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
    CountNgrams(&input_file);
  }

  return 0;
}
