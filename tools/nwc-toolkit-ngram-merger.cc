// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <utility>

#include <nwc-toolkit/heap-queue.h>
#include <nwc-toolkit/input-file.h>
#include <nwc-toolkit/output-file.h>

#define NWC_TOOLKIT_ERROR(fmt, ...) \
  error_at_line(-(__LINE__), errno, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

namespace {

enum { OUTPUT_BUF_LENGTH_THRESHOLD = (1 << 16) - (1 << 10) };

long long freq_threshold = 0;
nwc_toolkit::String output_file_name;
bool is_help_mode = false;

void ParseOptions(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "threshold", 1, NULL, 'n' },
    { "output", 1, NULL, 'o' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, '\0' }
  };

  int value;
  while ((value = ::getopt_long(argc, argv,
      "n:o:h", long_options, NULL)) != -1) {
    switch (value) {
      case 'n': {
        char *end_of_value;
        freq_threshold = std::strtoll(optarg, &end_of_value, 10);
        if ((*end_of_value != '\0') || (freq_threshold < 0)) {
          NWC_TOOLKIT_ERROR("invalid threshold: %s", optarg);
        }
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
      "  -n, --threshold=[N]  "
      "cut off n-grams whose frequencies are less than N\n"
      "  -o, --output=[FILE]  write result to FILE (default: stdout)\n"
      "  -h, --help           print this help\n"
      << std::flush;
}

typedef std::pair<nwc_toolkit::String, nwc_toolkit::InputFile> Pair;

class LessThan {
 public:
  bool operator()(const Pair *lhs, const Pair *rhs) const {
    return lhs->first < rhs->first;
  }
};

void AppendToOutput(const nwc_toolkit::String &ngram, long long freq,
    nwc_toolkit::StringBuilder *output_buf) {
  char freq_buf[32];
  int length = std::sprintf(freq_buf, "%lld\n", freq);
  output_buf->Append(ngram).Append(freq_buf, length);
}

bool Merge(int num_file_names, char *file_names[],
    nwc_toolkit::OutputFile *output_file) {
  std::time_t start_time = std::time(NULL);

  nwc_toolkit::HeapQueue<Pair *, LessThan> queue;

  Pair *pairs = new Pair[num_file_names];
  for (int i = 0; i < num_file_names; ++i) {
    if (!pairs[i].second.Open(file_names[i])) {
      NWC_TOOLKIT_ERROR("failed to open file: %s", file_names[i]);
    }
    if (pairs[i].second.ReadLine(&pairs[i].first)) {
      queue.Enqueue(&pairs[i]);
    }
  }

  nwc_toolkit::StringBuilder output_buf;
  nwc_toolkit::StringBuilder last_ngram;
  long long last_freq = -1;
  long long input_count = 0;
  long long output_count = 0;
  while (!queue.is_empty()) {
    Pair *pair = queue.top();

    nwc_toolkit::String delim = pair->first.FindLastOf('\t');
    nwc_toolkit::String ngram(pair->first.begin(), delim.end());
    long long freq = std::strtoll(delim.end(), NULL, 10);
    ++input_count;

    if (ngram != last_ngram.str()) {
      if (last_freq >= freq_threshold) {
        AppendToOutput(last_ngram.str(), last_freq, &output_buf);
        if (output_buf.length() > OUTPUT_BUF_LENGTH_THRESHOLD) {
          if (!output_file->Write(output_buf.str())) {
            NWC_TOOLKIT_ERROR("failed to write result");
          }
          output_buf.Clear();
        }
        ++output_count;
      }
      last_ngram = ngram;
      last_freq = freq;
    } else {
      last_freq += freq;
    }

    if (pair->second.ReadLine(&pair->first)) {
      queue.Replace(pair);
    } else {
      queue.Dequeue();
    }

    if ((input_count % 1000000) == 0) {
      std::cerr << '\r' << "input: " << input_count
          << ", output: " << output_count << " ("
          << std::fixed << std::setprecision(2)
          << (100.0 * output_count / input_count) << "%)"
          << " (" << (std::time(NULL) - start_time) << "sec)";
    }
  }
  if (last_freq >= freq_threshold) {
    AppendToOutput(last_ngram.str(), last_freq, &output_buf);
    ++output_count;
  }
  if (!output_file->Write(output_buf.str())) {
    NWC_TOOLKIT_ERROR("failed to write result");
  }
  std::cerr << '\r' << "input: " << input_count
      << ", output: " << output_count << " ("
      << std::fixed << std::setprecision(2)
      << ((input_count != 0) ? (100.0 * output_count / input_count) : 0.0)
      << "%) (" << (std::time(NULL) - start_time) << "sec)" << std::endl;
  delete [] pairs;
  return true;
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
    char *input_file_names[] = { NULL };
    Merge(1, input_file_names, &output_file);
  } else {
    Merge(argc - optind, argv + optind, &output_file);
  }

  return 0;
}
