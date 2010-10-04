// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/text-archive-entry.h>

int main() {
  nwc_toolkit::OutputFile output_file;
  assert(output_file.Open("test-text-archive-entry.dat"));
  assert(output_file.Write("1st line\n2nd line\n3rd line\n\n"));
  assert(output_file.Close());

  nwc_toolkit::TextArchiveEntry entry;

  assert(entry.num_lines() == 0);

  nwc_toolkit::InputFile input_file;
  assert(input_file.Open("test-text-archive-entry.dat"));
  assert(entry.Read(&input_file));
  assert(entry.num_lines() == 3);
  assert(entry.line(0) == "1st line");
  assert(entry.line(1) == "2nd line");
  assert(entry.line(2) == "3rd line");
  assert(input_file.Close());

  assert(output_file.Open("test-text-archive-entry.dat"));
  assert(entry.Write(&output_file));
  assert(output_file.Close());

  entry.Clear();

  assert(entry.num_lines() == 0);

  assert(input_file.Open("test-text-archive-entry.dat"));
  assert(entry.Read(&input_file));
  assert(entry.num_lines() == 3);
  assert(entry.line(0) == "1st line");
  assert(entry.line(1) == "2nd line");
  assert(entry.line(2) == "3rd line");
  assert(entry.Read(&input_file) == false);
  assert(input_file.Close());

  return 0;
}
