// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <nwc-toolkit/cetr-cluster.h>
#include <nwc-toolkit/cetr-document.h>
#include <nwc-toolkit/character-reference.h>

#define NWC_TOOLKIT_ERROR(fmt, ...) \
  error_at_line(-(__LINE__), errno, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

namespace {

enum InputFormat {
  HTML_ARCHIVE,
  SINGLE_HTML_DOCUMENT,
  DEFAULT_INPUT_FORMAT = HTML_ARCHIVE
};

enum {
  MIN_NUM_CHARS_THRESHOLD = nwc_toolkit::CetrDocument::MIN_NUM_CHARS_THRESHOLD,
  MAX_NUM_CHARS_THRESHOLD = 1 << 12,
  DEFAULT_NUM_CHARS_THRESHOLD =
      nwc_toolkit::CetrDocument::DEFAULT_NUM_CHARS_THRESHOLD
};

enum {
  MIN_WINDOW_SIZE = nwc_toolkit::CetrDocument::MIN_WINDOW_SIZE,
  MAX_WINDOW_SIZE = 16,
  DEFAULT_WINDOW_SIZE = nwc_toolkit::CetrDocument::DEFAULT_WINDOW_SIZE
};

enum {
  MIN_NUM_CLUSTERS = nwc_toolkit::CetrCluster::MIN_NUM_CLUSTERS,
  MAX_NUM_CLUSTERS = 16,
  DEFAULT_NUM_CLUSTERS = nwc_toolkit::CetrCluster::DEFAULT_NUM_CLUSTERS
};

enum OutputFormat {
  TEXT_DOCUMENT,
  XML_DOCUMENT,
  DEFAULT_OUTPUT_FORMAT = TEXT_DOCUMENT
};

InputFormat input_format = DEFAULT_INPUT_FORMAT;
int num_chars_threshold = DEFAULT_NUM_CHARS_THRESHOLD;
int window_size = DEFAULT_WINDOW_SIZE;
int num_clusters = DEFAULT_NUM_CLUSTERS;
OutputFormat output_format = DEFAULT_OUTPUT_FORMAT;
nwc_toolkit::String output_file_name;
bool is_help_mode = false;

int ParseIntegerValue(const char *arg, long min_value, long max_value) {
  char *end_of_value;
  long value = std::strtol(optarg, &end_of_value, 10);
  if (*end_of_value != '\0' || (value < min_value) || (value > max_value)) {
    return -1;
  }
  return static_cast<int>(value);
}

void ParseOptions(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "archive", 0, NULL, 'a' },
    { "single", 0, NULL, 's' },
    { "chars", 1, NULL, 'n' },
    { "window", 1, NULL, 'w' },
    { "clusters", 1, NULL, 'c' },
    { "text", 0, NULL, 't' },
    { "xml", 0, NULL, 'x' },
    { "output", 1, NULL, 'o' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, '\0' }
  };

  int value;
  while ((value = ::getopt_long(argc, argv,
      "asn:w:c:txo:h", long_options, NULL)) != -1) {
    switch (value) {
      case 'a': {
        input_format = HTML_ARCHIVE;
        break;
      }
      case 's': {
        input_format = SINGLE_HTML_DOCUMENT;
        break;
      }
      case 'n': {
        num_chars_threshold = ParseIntegerValue(optarg,
            MIN_NUM_CHARS_THRESHOLD, MAX_NUM_CHARS_THRESHOLD);
        if (num_chars_threshold < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
        break;
      }
      case 'w': {
        window_size = ParseIntegerValue(optarg,
            MIN_WINDOW_SIZE, MAX_WINDOW_SIZE);
        if (window_size < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
        break;
      }
      case 'c': {
        num_clusters = ParseIntegerValue(optarg,
            MIN_NUM_CLUSTERS, MAX_NUM_CLUSTERS);
        if (num_clusters < 0) {
          NWC_TOOLKIT_ERROR("invalid argument: `%c', %s", value, optarg);
        }
        break;
      }
      case 't': {
        output_format = TEXT_DOCUMENT;
        break;
      }
      case 'x': {
        output_format = XML_DOCUMENT;
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
      "  -n, --chars=[N]      "
      "limit the number of chars in line to N (default: "
      << DEFAULT_NUM_CHARS_THRESHOLD << ")\n"
      "  -w, --window=[N]     "
      "limit the window size to N (default: "
      << DEFAULT_WINDOW_SIZE << ")\n"
      "  -c, --clusters=[N]   "
      "limit the number of clusters to N (default: "
      << DEFAULT_NUM_CLUSTERS << ")\n"
      "  -t, --text     output result in text-format (default)\n"
      "  -x, --xml      output result in xml-format\n"
      "  -o, --output=[FILE]  write result to FILE (default: stdout)\n"
      "  -h, --help     print this help\n"
      << std::flush;
}

void WriteXmlHeader(nwc_toolkit::OutputFile *output_file) {
  if (!output_file->Write(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      "<ResultSet>\n")) {
    NWC_TOOLKIT_ERROR("failed to write xml header");
  }
}

void WriteXmlFooter(nwc_toolkit::OutputFile *output_file) {
  if (!output_file->Write("</ResultSet>\n")) {
    NWC_TOOLKIT_ERROR("failed to write xml header");
  }
}

void GetLineTag(const nwc_toolkit::CetrDocument &doc,
    std::size_t line_id, nwc_toolkit::StringBuilder *result) {
  enum { MAX_LENGTH = 256 };
  result->Reserve(result->length() + MAX_LENGTH);
  int length = std::snprintf(result->buf() + result->length(), MAX_LENGTH,
      "<Line is_content=\"%s\" num_tags=\"%zd\" num_chars=\"%zd\""
      " tag_ratio=\"%f\" smoothed_tag_ratio=\"%f\""
      " derivate=\"%f\" smoothed_derivate=\"%f\">\n",
      (doc.is_content(line_id) ? "yes" : " no"),
      doc.line(line_id).num_tags(), doc.line(line_id).num_text_chars(),
      doc.tag_ratio(line_id), doc.smoothed_tag_ratio(line_id),
      doc.derivate(line_id), doc.smoothed_derivate(line_id));
  if ((length < 0) || (length >= MAX_LENGTH)) {
    NWC_TOOLKIT_ERROR("too long line tag");
  }
  result->Resize(result->length() + length);
}

void GetXmlResult(const nwc_toolkit::CetrDocument &doc,
    nwc_toolkit::StringBuilder *result) {
  result->Append("<Result>\n");
  for (std::size_t i = 0; i < doc.num_lines(); ++i) {
    GetLineTag(doc, i, result);
    const nwc_toolkit::CetrLine &line = doc.line(i);
    for (std::size_t j = 0; j < line.num_units(); ++j) {
      const nwc_toolkit::CetrUnit &unit = line.unit(j);
      result->Append("<Unit type=\"");
      switch (unit.type()) {
        case nwc_toolkit::CetrUnit::TAG_UNIT: {
          result->Append("tag");
          break;
        }
        case nwc_toolkit::CetrUnit::TEXT_UNIT: {
          result->Append("text");
          break;
        }
        case nwc_toolkit::CetrUnit::INVISIBLE_UNIT: {
          result->Append("invisible");
          break;
        }
        default: {
          NWC_TOOLKIT_ERROR("invalid unit type: %d", unit.type());
        }
      }
      result->Append("\">");
      nwc_toolkit::CharacterReference::Encode(unit.content(), result);
      result->Append("</Unit>\n");
    }
    result->Append("</Line>\n");
  }
  result->Append("</Result>\n");
}

void AppendByteToTextResult(char byte, nwc_toolkit::StringBuilder *result) {
  if (nwc_toolkit::IsSpace()(byte)) {
    // Successive white-spaces are replaced with a single white-space or
    // end-of-line mark.
    char last_byte = !result->is_empty()
        ? ((*result)[result->length() - 1]) : '\n';
    if ((byte == '\r') || (byte == '\n')) {
      if (!nwc_toolkit::IsSpace()(last_byte)) {
        result->Append('\n');
      } else if (last_byte != '\n') {
        (*result)[result->length() - 1] = '\n';
      }
    } else if (!nwc_toolkit::IsSpace()(last_byte)) {
      result->Append(' ');
    }
  } else {
    result->Append(byte);
  }
}

void GetTextResult(const nwc_toolkit::CetrDocument &doc,
    nwc_toolkit::StringBuilder *result) {
  for (std::size_t i = 0; i < doc.num_lines(); ++i) {
    if (!doc.is_content(i)) {
      continue;
    }
    const nwc_toolkit::CetrLine &line = doc.line(i);
    for (std::size_t j = 0; j < line.num_units(); ++j) {
      const nwc_toolkit::CetrUnit &unit = line.unit(j);
      if (unit.type() == nwc_toolkit::CetrUnit::TEXT_UNIT) {
        const nwc_toolkit::String &text = unit.content();
        for (std::size_t i = 0; i < text.length(); ++i) {
          AppendByteToTextResult(text[i], result);
        }
      }
    }
  }
  AppendByteToTextResult('\n', result);
  result->Append('\n');
}

void GetResult(const nwc_toolkit::CetrDocument &doc,
    nwc_toolkit::StringBuilder *result) {
  switch (output_format) {
    case TEXT_DOCUMENT: {
      GetTextResult(doc, result);
      break;
    }
    case XML_DOCUMENT: {
      GetXmlResult(doc, result);
      break;
    }
    default: {
      NWC_TOOLKIT_ERROR("invalid output format");
    }
  }
}

void ExtractContentsFromHtmlArchvie(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  std::time_t start_time = std::time(NULL);

  nwc_toolkit::HtmlDocument html_doc;
  nwc_toolkit::CetrDocument cetr_doc;
  nwc_toolkit::StringBuilder result;

  long long num_entries = 0;
  long long status_error_count = 0;
  long long parse_error_count = 0;
  nwc_toolkit::HtmlArchiveEntry entry;
  while (entry.Read(input_file)) {
    html_doc.Clear();
    cetr_doc.Clear();
    result.Clear();

    if (entry.status_code() != 200) {
      ++status_error_count;
    } else if (!html_doc.Parse(entry)) {
      ++parse_error_count;
    } else {
      cetr_doc.Parse(html_doc);
    }

    GetResult(cetr_doc, &result);
    if (!output_file->Write(result.str())) {
      NWC_TOOLKIT_ERROR("failed to write result");
    }

    if ((++num_entries % 100) == 0) {
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
      << ((num_entries != 0) ? (100.0 * status_error_count / num_entries) : 0.0)
      << "%) / " << parse_error_count << " ("
      << std::fixed << std::setw(5) << std::setprecision(2)
      << ((num_entries != 0) ? (100.0 * parse_error_count / num_entries) : 0.0)
      << "%) / " << num_entries
      << " (" << (std::time(NULL) - start_time) << "sec)" << std::endl;
}

void ExtractContentsFromSingleHtmlDocument(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  nwc_toolkit::String line;
  nwc_toolkit::StringBuilder body;
  while (input_file->ReadLine(&line)) {
    body.Append(line);
  }

  nwc_toolkit::HtmlDocument html_doc;
  if (!html_doc.Parse(body.str())) {
    NWC_TOOLKIT_ERROR("failed to parse html document");
  }

  nwc_toolkit::CetrDocument cetr_doc;
  cetr_doc.Parse(html_doc);

  nwc_toolkit::StringBuilder result;
  GetResult(cetr_doc, &result);
  if (!output_file->Write(result.str())) {
    NWC_TOOLKIT_ERROR("failed to write result");
  }
}

void ExtractContents(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  switch (output_format) {
    case TEXT_DOCUMENT: {
      break;
    }
    case XML_DOCUMENT: {
      WriteXmlHeader(output_file);
      break;
    }
    default: {
      NWC_TOOLKIT_ERROR("invalid output format");
    }
  }

  switch (input_format) {
    case HTML_ARCHIVE: {
      ExtractContentsFromHtmlArchvie(input_file, output_file);
      break;
    }
    case SINGLE_HTML_DOCUMENT: {
      ExtractContentsFromSingleHtmlDocument(input_file, output_file);
      break;
    }
    default: {
      NWC_TOOLKIT_ERROR("invalid input format");
    }
  }

  if (output_format == XML_DOCUMENT) {
    WriteXmlFooter(output_file);
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
    ExtractContents(&input_file, &output_file);
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
    ExtractContents(&input_file, &output_file);
  }

  return 0;
}
