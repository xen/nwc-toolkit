// Copyright 2010 Susumu Yata <syata@acm.org>

#include <errno.h>
#include <error.h>
#include <getopt.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <tr1/unordered_set>

#include <nwc-toolkit/html-reducer.h>
#include <nwc-toolkit/string-hash.h>

#define NWC_TOOLKIT_ERROR(fmt, ...) \
  error_at_line(-(__LINE__), errno, __FILE__, __LINE__, fmt, ## __VA_ARGS__)

namespace {

typedef std::tr1::unordered_set<nwc_toolkit::String,
    nwc_toolkit::StringHash> HostSet;

enum InputFormat {
  HTML_ARCHIVE,
  SINGLE_HTML_DOCUMENT,
  DEFAULT_FORMAT = HTML_ARCHIVE
};

InputFormat input_format = DEFAULT_FORMAT;
bool needs_section_target = false;
bool unique_host = false;
nwc_toolkit::StringPool host_pool;
HostSet host_set;
nwc_toolkit::String output_file_name;
bool is_help_mode = false;

void ParseOptions(int argc, char *argv[]) {
  static const struct option long_options[] = {
    { "archive", 0, NULL, 'a' },
    { "single", 0, NULL, 's' },
    { "target", 0, NULL, 't' },
    { "unique", 0, NULL, 'u' },
    { "output", 1, NULL, 'o' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, '\0' }
  };

  int value;
  while ((value = ::getopt_long(argc, argv,
      "astuo:h", long_options, NULL)) != -1) {
    switch (value) {
      case 'a': {
        input_format = HTML_ARCHIVE;
        break;
      }
      case 's': {
        input_format = SINGLE_HTML_DOCUMENT;
        break;
      }
      case 't': {
        needs_section_target = true;
        break;
      }
      case 'u': {
        unique_host = true;
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
      "  -a, --archive  reduce HTML archives (default)\n"
      "  -s, --single   reduce HTML documents\n"
      "  -t, --target   extract HTML documents having section targets\n"
      "  -u, --unique   extract HTML documents having unique hosts\n"
      "  -o, --output=[FILE]  write result to FILE (default: stdout)\n"
      "  -h, --help     print this help\n"
      << std::flush;
}

nwc_toolkit::String Gethost(const nwc_toolkit::String &url) {
  nwc_toolkit::String delim = url.Find("://");
  if (delim.is_empty()) {
    return "";
  }
  nwc_toolkit::String host(delim.end(), url.end());
  delim = host.FindFirstOf(":/");
  if (delim.is_empty()) {
    return "";
  }
  host.set_end(delim.begin());
  return host;
}

void ReduceHtmlHeader(const nwc_toolkit::HtmlArchiveEntry &entry,
    const nwc_toolkit::String &body, nwc_toolkit::StringBuilder *header) {
  enum { HEADER_SIZE = 32 };
  header->Clear();
  header->Append(entry.url());
  header->Reserve(header->length() + HEADER_SIZE);
  int length = std::snprintf(header->buf() + header->length(), HEADER_SIZE,
      "\n%d\n0\n%zu\n", entry.status_code(), body.length());
  if ((length < 0) || (length >= HEADER_SIZE)) {
    NWC_TOOLKIT_ERROR("too long header");
  }
  header->Resize(header->length() + length);
}

void ReduceHtmlArchive(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  std::time_t start_time = std::time(NULL);

  nwc_toolkit::HtmlArchiveEntry entry;
  nwc_toolkit::StringBuilder last_host;

  nwc_toolkit::HtmlDocument doc;
  nwc_toolkit::StringBuilder body;
  nwc_toolkit::StringBuilder header;

  long long num_entries = 0;
  long long duplicate_count = 0;
  long long status_error_count = 0;
  long long parse_error_count = 0;
  long long output_count = 0;

  while (entry.Read(input_file)) {
    nwc_toolkit::String host = Gethost(entry.url());
    if (unique_host && host_set.find(host) != host_set.end()) {
      ++duplicate_count;
    } else if (entry.status_code() != 200) {
      ++status_error_count;
    } else if (!doc.Parse(entry)) {
      ++parse_error_count;
    } else {
      nwc_toolkit::HtmlReducer::Reduce(doc, &body);
      if (!needs_section_target ||
          !body.str().Find("<!-- google_ad_section_start").is_empty()) {
        ReduceHtmlHeader(entry, body.str(), &header);
        if (!output_file->Write(header.str()) ||
            !output_file->Write(body.str())) {
          NWC_TOOLKIT_ERROR("failed to write result");
        }
        ++output_count;
        if (unique_host) {
          host_set.insert(host_pool.Append(host));
          last_host = host;
        }
      }
    }

    if ((++num_entries % 100) == 0) {
      std::cerr << '\r' << status_error_count << " ("
          << std::fixed << std::setw(5) << std::setprecision(2)
          << (100.0 * status_error_count / num_entries)
          << "%) / " << parse_error_count << " ("
          << std::fixed << std::setw(5) << std::setprecision(2)
          << (100.0 * parse_error_count / num_entries)
          << "%) / " << output_count << " ("
          << std::fixed << std::setw(5) << std::setprecision(2)
          << (100.0 * output_count / num_entries)
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
      << "%) / " << output_count << " ("
      << std::fixed << std::setw(5) << std::setprecision(2)
      << ((num_entries != 0) ? (100.0 * output_count / num_entries) : 0.0)
      << "%) / " << num_entries
      << " (" << (std::time(NULL) - start_time) << "sec)" << std::endl;
}

void ReduceSingleHtmlDocument(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  nwc_toolkit::String line;
  nwc_toolkit::StringBuilder body;
  while (input_file->ReadLine(&line)) {
    body.Append(line);
  }

  nwc_toolkit::HtmlDocument doc;
  if (!doc.Parse(body.str())) {
    NWC_TOOLKIT_ERROR("failed to parse html document");
  }

  nwc_toolkit::HtmlReducer::Reduce(doc, &body);
  if (!output_file->Write(body.str())) {
    NWC_TOOLKIT_ERROR("failed to write result");
  }
}

void Reduce(nwc_toolkit::InputFile *input_file,
    nwc_toolkit::OutputFile *output_file) {
  switch (input_format) {
    case HTML_ARCHIVE: {
      ReduceHtmlArchive(input_file, output_file);
      break;
    }
    case SINGLE_HTML_DOCUMENT: {
      ReduceSingleHtmlDocument(input_file, output_file);
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
    Reduce(&input_file, &output_file);
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
    Reduce(&input_file, &output_file);
  }

  return 0;
}
