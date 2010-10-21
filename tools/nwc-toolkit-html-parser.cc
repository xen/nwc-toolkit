// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <cstdlib>
#include <iostream>

#include <nwc-toolkit/character-reference.h>
#include <nwc-toolkit/html-document.h>

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

void Parse(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  nwc_toolkit::String line;
  nwc_toolkit::StringBuilder body;
  while (input_file->ReadLine(&line)) {
    body.Append(line);
  }

  nwc_toolkit::HtmlDocument document;
  if (!document.Parse(body.str())) {
    NWC_TOOLKIT_ERROR("failed to parse document");
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
        NWC_TOOLKIT_ERROR("invalid document unit");
      }
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
    Parse(&input_file, &output_file);
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
    Parse(&input_file, &output_file);
  }

  return 0;
}
