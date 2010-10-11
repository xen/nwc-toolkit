// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cstdlib>
#include <iostream>

#include <nwc-toolkit/character-reference.h>
#include <nwc-toolkit/html-document.h>

namespace {

class HtmlParser {
 public:
  HtmlParser() {}
  ~HtmlParser() {}

  void ParseOptions(int *argc, char *argv[]);

  bool Parse(nwc_toolkit::InputFile *input_file,
      nwc_toolkit::OutputFile *output_file);

 private:
  // Disallows copy and assignment.
  HtmlParser(const HtmlParser &);
  HtmlParser &operator=(const HtmlParser &);
};

void HtmlParser::ParseOptions(int *argc, char *argv[]) {
  int new_argc = 1;
  for (int i = 1; i < *argc; ++i) {
    nwc_toolkit::String arg = argv[i];
    argv[new_argc++] = argv[i];
  }
  *argc = new_argc;
}

bool HtmlParser::Parse(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  nwc_toolkit::String line;
  nwc_toolkit::StringBuilder body;
  while (input_file->ReadLine(&line)) {
    body.Append(line);
  }

  nwc_toolkit::HtmlDocument document;
  if (!document.Parse(body.str())) {
    std::cerr << "error: failed to parse document" << std::endl;
    return false;
  }

  nwc_toolkit::StringBuilder name_buf;
  nwc_toolkit::StringBuilder value_buf;
  for (std::size_t i = 0; i < document.num_units(); ++i) {
    const nwc_toolkit::HtmlDocumentUnit &unit = document.unit(i);
    switch (unit.type()) {
      case nwc_toolkit::HtmlDocumentUnit::TEXT_UNIT: {
        std::cout << "Text: " << unit.text_content().length() << "bytes\n";
        break;
      }
      case nwc_toolkit::HtmlDocumentUnit::TAG_UNIT: {
        std::cout << "Tag: <";
        if (unit.is_end_tag()) {
          std::cout << '/';
        }
        std::cout << unit.tag_name();
        for (std::size_t j = 0; j < unit.num_attributes(); ++j) {
          name_buf.Clear();
          nwc_toolkit::CharacterReference::Decode(
              unit.attribute(j).name(), &name_buf);
          value_buf.Clear();
          nwc_toolkit::CharacterReference::Decode(
              unit.attribute(j).value(), &value_buf);
          std::cout << ' ' << name_buf << "=\"" << value_buf << '"';
        }
        if (unit.is_empty_element_tag()) {
          std::cout << " /";
        }
        std::cout << ">\n";
        break;
      }
      case nwc_toolkit::HtmlDocumentUnit::COMMENT_UNIT: {
        std::cout << "Comment: " << unit.comment().length() << "bytes\n";
        break;
      }
      case nwc_toolkit::HtmlDocumentUnit::OTHER_UNIT: {
        std::cout << "Other: " << unit.other_content().length() << "bytes\n";
        break;
      }
      default: {
        std::cerr << "error: undefined document unit" << std::endl;
        return false;
      }
    }
  }
  return true;
}

void PrintHelp(const char *command) {
  std::cerr << "Usage: " << command << " [OPTION]... [FILE]...\n\n"
      "Options:\n"
      "  --output=[FILE]  output Parseed texts to this file\n"
      "  --help      print this help\n"
      << std::flush;
}

}  // namespace

int main(int argc, char *argv[]) {
  HtmlParser html_parser;
  html_parser.ParseOptions(&argc, argv);

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
    } else if (!html_parser.Parse(&input_file, &output_file)) {
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
    } else if (!html_parser.Parse(&input_file, &output_file)) {
      return -4;
    }
  }

  return 0;
}
