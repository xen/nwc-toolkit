// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/cetr-line.h>

int main() {
  nwc_toolkit::CetrLine line;

  assert(line.num_units() == 0);
  assert(line.num_tags() == 0);
  assert(line.num_chars() == 0);
  assert(line.num_text_chars() == 0);

  line.set_num_units(1);
  line.set_num_tags(2);
  line.set_num_chars(3);
  line.set_num_text_chars(4);

  assert(line.num_units() == 1);
  assert(line.num_tags() == 2);
  assert(line.num_chars() == 3);
  assert(line.num_text_chars() == 4);

  nwc_toolkit::CetrLine line_copy(line);

  assert(line_copy.num_units() == 1);
  assert(line_copy.num_tags() == 2);
  assert(line_copy.num_chars() == 3);
  assert(line_copy.num_text_chars() == 4);

  nwc_toolkit::CetrLine line_copy2;
  line_copy2 = line;

  assert(line_copy2.num_units() == 1);
  assert(line_copy2.num_tags() == 2);
  assert(line_copy2.num_chars() == 3);
  assert(line_copy2.num_text_chars() == 4);

  line.Clear();

  assert(line.num_units() == 0);
  assert(line.num_tags() == 0);
  assert(line.num_chars() == 0);
  assert(line.num_text_chars() == 0);

  return 0;
}
