// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <nwc-toolkit/html-archive-entry.h>
#include <nwc-toolkit/html-document.h>

namespace {

class TextExtractor {
 public:
  enum ExtractionMode {
    HTML_ARCHIVE_MODE,
    HTML_DOCUMENT_MODE
  };

  TextExtractor() : mode_(HTML_ARCHIVE_MODE), max_num_entries_(0) {}
  ~TextExtractor() {}

  ExtractionMode mode() const {
    return mode_;
  }
  long long max_num_entries() const {
    return max_num_entries_;
  }

  void set_mode(ExtractionMode mode) {
    mode_ = mode;
  }
  void set_max_num_entries(long long max_num_entries) {
    max_num_entries_ = max_num_entries;
  }

  void ParseOptions(int *argc, char *argv[]);

  bool Extract(nwc_toolkit::InputFile *input_file,
      nwc_toolkit::OutputFile *output_file);

 private:
  ExtractionMode mode_;
  long long max_num_entries_;

  bool ExtractFromHtmlArchvie(nwc_toolkit::InputFile *input_file,
      nwc_toolkit::OutputFile *output_file);
  bool ExtractFromHtmlDocument(nwc_toolkit::InputFile *input_file,
      nwc_toolkit::OutputFile *output_file);

  // Disallows copy and assignment.
  TextExtractor(const TextExtractor &);
  TextExtractor &operator=(const TextExtractor &);
};

void TextExtractor::ParseOptions(int *argc, char *argv[]) {
  int new_argc = 1;
  for (int i = 1; i < *argc; ++i) {
    nwc_toolkit::String arg = argv[i];
    if (arg.StartsWith("--")) {
      arg = arg.SubString(2);
      if (arg.Compare("archive", nwc_toolkit::ToLower()) == 0) {
        set_mode(HTML_ARCHIVE_MODE);
      } else if (arg.Compare("document", nwc_toolkit::ToLower()) == 0) {
        set_mode(HTML_DOCUMENT_MODE);
      } else {
        char *value_end;
        long long value = std::strtoll(arg.ptr(), &value_end, 10);
        if ((*value_end == '\0') && (value >= 0)) {
          set_max_num_entries(value);
        } else {
          argv[new_argc++] = argv[i];
        }
      }
    } else {
      argv[new_argc++] = argv[i];
    }
  }
  *argc = new_argc;
}

bool TextExtractor::Extract(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  switch (mode()) {
    case HTML_ARCHIVE_MODE: {
      return ExtractFromHtmlArchvie(input_file, output_file);
    }
    case HTML_DOCUMENT_MODE: {
      return ExtractFromHtmlDocument(input_file, output_file);
    }
    default: {
      std::cerr << "error: unknown extraction mode: " << mode() << std::endl;
      return false;
    }
  }
}

bool TextExtractor::ExtractFromHtmlArchvie(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  nwc_toolkit::HtmlDocument document;
  nwc_toolkit::StringBuilder text;

  std::time_t start_time = std::time(NULL);

  long long num_entries = 0;
  long long status_error_count = 0;
  long long parse_error_count = 0;
  nwc_toolkit::HtmlArchiveEntry entry;
  while (entry.Read(input_file)) {
    if (entry.status_code() != 200) {
      ++status_error_count;
    } else if (!document.Parse(entry)) {
      ++parse_error_count;
    } else {
      text.Clear();
      document.ExtractText(&text);
      text.Append('\n');
      output_file->Write(text.str());
    }

    if (++num_entries == max_num_entries()) {
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
      << " (" << (std::time(NULL) - start_time) << "sec)";
  return true;
}

bool TextExtractor::ExtractFromHtmlDocument(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  nwc_toolkit::String line;
  nwc_toolkit::StringBuilder body;
  while (input_file->ReadLine(&line)) {
    body.Append(line);
  }

  nwc_toolkit::HtmlDocument document;
  nwc_toolkit::StringBuilder text;
  if (document.Parse(body.str())) {
    document.ExtractText(&text);
    text.Append('\n');
    output_file->Write(text.str());
  }
  return true;
}

void PrintHelp(const char *command) {
  std::cerr << "Usage: " << command << " [OPTION]... [FILE]...\n\n"
      "Options:\n"
      "  --archive   handle each file as an HTML archive\n"
      "  --document  handle each file as an HTML document\n"
      "  --help      print this help\n"
      "  --output=[FILE]  output extracted texts to this file\n"
      "  --N         limit the number of entries to be processed\n"
      << std::flush;
}

}  // namespace

int main(int argc, char *argv[]) {
  TextExtractor text_extractor;
  text_extractor.ParseOptions(&argc, argv);

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
    } else if (text_extractor.Extract(&input_file, &output_file)) {
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
    } else if (text_extractor.Extract(&input_file, &output_file)) {
      return -4;
    }
  }

  return 0;
}
