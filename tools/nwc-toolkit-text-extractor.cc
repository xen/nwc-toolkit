// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <nwc-toolkit/html-archive-entry.h>
#include <nwc-toolkit/html-document.h>
#include <nwc-toolkit/text-filter.h>
#include <nwc-toolkit/unicode-normalizer.h>

namespace {

class TextExtractor {
 public:
  enum InputFileFormat {
    HTML_ARCHIVE,
    HTML_DOCUMENT
  };

  TextExtractor()
      : format_(HTML_ARCHIVE),
        max_num_entries_(0),
        form_(nwc_toolkit::UnicodeNormalizer::DEFAULT_FORM),
        handler_(nwc_toolkit::UnicodeNormalizer::DEFAULT_HANDLER),
        with_unicode_normalization_(false),
        with_text_filter_(false) {}
  ~TextExtractor() {}

  InputFileFormat format() const {
    return format_;
  }
  long long max_num_entries() const {
    return max_num_entries_;
  }
  nwc_toolkit::UnicodeNormalizer::NormalizationForm form() const {
    return form_;
  }
  nwc_toolkit::UnicodeNormalizer::IllegalInputHandler handler() const {
    return handler_;
  }
  bool with_unicode_normalization() const {
    return with_unicode_normalization_;
  }
  bool with_text_filter() const {
    return with_text_filter_;
  }

  void set_format(InputFileFormat format) {
    format_ = format;
  }
  void set_max_num_entries(long long max_num_entries) {
    max_num_entries_ = max_num_entries;
  }
  void set_form(nwc_toolkit::UnicodeNormalizer::NormalizationForm form) {
    form_ = form;
  }
  void set_handler(
      nwc_toolkit::UnicodeNormalizer::IllegalInputHandler handler) {
    handler_ = handler;
  }
  void set_with_unicode_normalization(bool flag) {
    with_unicode_normalization_ = flag;
  }
  void set_with_text_filter(bool flag) {
    with_text_filter_ = flag;
  }

  void ParseOptions(int *argc, char *argv[]);

  bool Extract(nwc_toolkit::InputFile *input_file,
      nwc_toolkit::OutputFile *output_file);

 private:
  InputFileFormat format_;
  long long max_num_entries_;
  nwc_toolkit::UnicodeNormalizer::NormalizationForm form_;
  nwc_toolkit::UnicodeNormalizer::IllegalInputHandler handler_;
  bool with_unicode_normalization_;
  bool with_text_filter_;

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
    if (arg.Compare("--arhive", nwc_toolkit::ToLower()) == 0) {
      set_format(HTML_ARCHIVE);
    } else if (arg.Compare("--document", nwc_toolkit::ToLower()) == 0) {
      set_format(HTML_DOCUMENT);
    } else if (arg.Compare("--NFC", nwc_toolkit::ToLower()) == 0) {
      set_with_unicode_normalization(true);
      set_form(nwc_toolkit::UnicodeNormalizer::NFC);
    } else if (arg.Compare("--NFD", nwc_toolkit::ToLower()) == 0) {
      set_with_unicode_normalization(true);
      set_form(nwc_toolkit::UnicodeNormalizer::NFD);
    } else if (arg.Compare("--NFKC", nwc_toolkit::ToLower()) == 0) {
      set_with_unicode_normalization(true);
      set_form(nwc_toolkit::UnicodeNormalizer::NFKC);
    } else if (arg.Compare("--NFKD", nwc_toolkit::ToLower()) == 0) {
      set_with_unicode_normalization(true);
      set_form(nwc_toolkit::UnicodeNormalizer::NFKD);
    } else if (arg.Compare("--keep", nwc_toolkit::ToLower()) == 0) {
      set_with_unicode_normalization(true);
      set_handler(
          nwc_toolkit::UnicodeNormalizer::KEEP_REPLACEMENT_CHARACTERS);
    } else if (arg.Compare("--remove", nwc_toolkit::ToLower()) == 0) {
      set_with_unicode_normalization(true);
      set_handler(
          nwc_toolkit::UnicodeNormalizer::REMOVE_REPLACEMENT_CHARACTERS);
    } else if (arg.Compare("--filter", nwc_toolkit::ToLower()) == 0) {
      set_with_text_filter(true);
    } else if (arg.StartsWith("--")) {
      char *value_end;
      long long value = std::strtoll(arg.ptr() + 2, &value_end, 10);
      if ((*value_end == '\0') && (value >= 0)) {
        set_max_num_entries(value);
      } else {
        argv[new_argc++] = argv[i];
      }
    } else {
      argv[new_argc++] = argv[i];
    }
  }
  *argc = new_argc;
}

bool TextExtractor::Extract(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  switch (format()) {
    case HTML_ARCHIVE: {
      return ExtractFromHtmlArchvie(input_file, output_file);
    }
    case HTML_DOCUMENT: {
      return ExtractFromHtmlDocument(input_file, output_file);
    }
    default: {
      std::cerr << "error: unknown input file format: "
          << format() << std::endl;
      return false;
    }
  }
}

bool TextExtractor::ExtractFromHtmlArchvie(nwc_toolkit::InputFile *input_file,
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

      if (with_unicode_normalization()) {
        normalized_text.Clear();
        if (!nwc_toolkit::UnicodeNormalizer::Normalize(
            form_, handler_, temp->str(), &normalized_text)) {
          ++parse_error_count;
        }
        temp = &normalized_text;
      }

      if (with_text_filter()) {
        filtered_text.Clear();
        nwc_toolkit::TextFilter::Filter(temp->str(), &filtered_text);
        temp = &filtered_text;
      }
    }
    temp->Append('\n');
    output_file->Write(temp->str());

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
      << " (" << (std::time(NULL) - start_time) << "sec)" << std::endl;
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
    if (!output_file->Write(text.str())) {
      std::cerr << "error: failed to output text" << std::endl;
      return false;
    }
  }
  return true;
}

void PrintHelp(const char *command) {
  std::cerr << "Usage: " << command << " [OPTION]... [FILE]...\n\n"
      "Options:\n"
      "  --archive   handle each file as an HTML archive (default)\n"
      "  --document  handle each file as an HTML document\n"
      "  --N         limit the number of entries to be processed\n"
      "  --NFC       Normalization Form C\n"
      "  --NFD       Normalization Form D\n"
      "  --NFKC      Normalization Form KC\n"
      "  --NFKD      Normalization Form KD\n"
      "              C = Composition, D = Decomposition, K = Compatibility\n"
      "  --keep      keep replacement characters (default)\n"
      "  --remove    remove replacement characters\n"
      "  --filter    apply filter to text\n"
      "  --output=[FILE]  output extracted texts to this file\n"
      "  --help      print this help\n"
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
