// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/cetr-point.h>

int main() {
  nwc_toolkit::CetrPoint point;

  assert(point.x() == 0.0);
  assert(point.y() == 0.0);

  point.set_x(1.23);
  point.set_y(12.3);

  assert(point.x() == 1.23);
  assert(point.y() == 12.3);

  nwc_toolkit::CetrPoint point_copy(point);

  assert(point_copy.x() == 1.23);
  assert(point_copy.y() == 12.3);

  nwc_toolkit::CetrPoint point_copy2;
  point_copy2 = point;

  assert(point_copy2.x() == 1.23);
  assert(point_copy2.y() == 12.3);

  point.Clear();

  assert(point.x() == 0.0);
  assert(point.y() == 0.0);

  return 0;
}
