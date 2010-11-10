// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/html-attribute.h>

int main() {
  nwc_toolkit::HtmlAttribute attribute;

  assert(attribute.name().is_empty());
  assert(attribute.value().is_empty());

  attribute.set_name("name");
  attribute.set_value("value");

  assert(attribute.name() == "name");
  assert(attribute.value() == "value");

  nwc_toolkit::HtmlAttribute attribute_copy = attribute;

  assert(attribute_copy.name() == "name");
  assert(attribute_copy.value() == "value");

  nwc_toolkit::HtmlAttribute attribute_assign;
  attribute_assign = attribute;

  assert(attribute_assign.name() == "name");
  assert(attribute_assign.value() == "value");

  attribute.Clear();

  assert(attribute.name().is_empty());
  assert(attribute.value().is_empty());

  return 0;
}
