// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/html-archive-entry.h>

int main() {
  nwc_toolkit::HtmlArchiveEntry entry;

  assert(entry.url().is_empty());
  assert(entry.status_code() == 0);
  assert(entry.header().is_empty());
  assert(entry.body().is_empty());

  entry.set_url("http://www.example.com/");
  entry.set_status_code(200);
  entry.set_header("Content-type: text/html\n");
  entry.set_body("<html></html>\n");

  assert(entry.url() == "http://www.example.com/");
  assert(entry.status_code() == 200);
  assert(entry.header() == "Content-type: text/html\n");
  assert(entry.body() == "<html></html>\n");

  nwc_toolkit::OutputFile output_file;
  assert(output_file.Open("test-html-archive-entry.dat"));
  assert(entry.Write(&output_file));
  assert(output_file.Close());

  entry.Clear();

  assert(entry.url().is_empty());
  assert(entry.status_code() == 0);
  assert(entry.header().is_empty());
  assert(entry.body().is_empty());

  nwc_toolkit::InputFile input_file;
  assert(input_file.Open("test-html-archive-entry.dat"));
  assert(entry.Read(&input_file));
  assert(input_file.Close());

  assert(entry.url() == "http://www.example.com/");
  assert(entry.status_code() == 200);
  assert(entry.header() == "Content-type: text/html\n");
  assert(entry.body() == "<html></html>\n");

  return 0;
}
