// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/cetr-unit.h>

int main() {
  nwc_toolkit::CetrUnit unit;

  assert(unit.type() == nwc_toolkit::CetrUnit::INVISIBLE_UNIT);
  assert(&unit.unit() == NULL);
  assert(unit.content().is_empty());

  const nwc_toolkit::HtmlUnit html_unit;

  unit.set_type(nwc_toolkit::CetrUnit::INVISIBLE_UNIT);
  unit.set_unit(html_unit);
  unit.set_content("Text");

  assert(unit.type() == nwc_toolkit::CetrUnit::INVISIBLE_UNIT);
  assert(&unit.unit() == &html_unit);
  assert(unit.content() == "Text");

  nwc_toolkit::CetrUnit unit_copy(unit);

  assert(unit_copy.type() == nwc_toolkit::CetrUnit::INVISIBLE_UNIT);
  assert(&unit_copy.unit() == &html_unit);
  assert(unit_copy.content() == "Text");

  nwc_toolkit::CetrUnit unit_copy2;
  unit_copy2 = unit;

  assert(unit_copy2.type() == nwc_toolkit::CetrUnit::INVISIBLE_UNIT);
  assert(&unit_copy2.unit() == &html_unit);
  assert(unit_copy2.content() == "Text");

  unit.Clear();

  assert(unit.type() == nwc_toolkit::CetrUnit::INVISIBLE_UNIT);
  assert(&unit.unit() == NULL);
  assert(unit.content().is_empty());

  return 0;
}
