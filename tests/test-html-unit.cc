// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/html-unit.h>

int main() {
  nwc_toolkit::HtmlUnit unit;

  assert(unit.type() == nwc_toolkit::HtmlUnit::UNDEFINED_UNIT);
  assert(unit.src().is_empty());

  unit.set_src("SRC");
  assert(unit.src() == "SRC");

  unit.Clear();

  assert(unit.src().is_empty());

  unit.set_type(nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(unit.type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(unit.is_cdata_section() == false);
  assert(unit.text_content().is_empty());

  unit.set_cdata_section_flag();
  assert(unit.is_cdata_section());

  unit.set_text_content("TEXT_CONTENT");
  assert(unit.text_content() == "TEXT_CONTENT");

  unit.Clear();

  unit.set_type(nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(unit.type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(unit.is_start_tag() == false);
  assert(unit.is_end_tag() == false);
  assert(unit.is_empty_element_tag() == false);
  assert(unit.tag_name().is_empty());
  assert(unit.num_attributes() == 0);

  unit.set_start_tag_flag();
  assert(unit.is_start_tag());
  assert(unit.is_end_tag() == false);
  assert(unit.is_empty_element_tag() == false);

  unit.set_end_tag_flag();
  assert(unit.is_end_tag());
  assert(unit.is_start_tag());
  assert(unit.is_empty_element_tag() == false);

  unit.set_empty_element_tag_flag();
  assert(unit.is_end_tag());
  assert(unit.is_start_tag());
  assert(unit.is_empty_element_tag());

  unit.set_tag_name("TAG_NAME");
  assert(unit.tag_name() == "TAG_NAME");

  nwc_toolkit::HtmlAttribute attribute;

  unit.set_attributes(&attribute, 1);

  assert(unit.num_attributes() == 1);
  assert(&unit.attribute(0) == &attribute);

  nwc_toolkit::HtmlUnit unit_copy = unit;
  assert(unit_copy.type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(unit_copy.is_end_tag());
  assert(unit_copy.is_start_tag());
  assert(unit_copy.is_empty_element_tag());
  assert(unit_copy.tag_name() == "TAG_NAME");
  assert(unit_copy.num_attributes() == 1);
  assert(&unit_copy.attribute(0) == &attribute);

  nwc_toolkit::HtmlUnit unit_assign;
  unit_assign = unit;
  assert(unit_assign.type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(unit_assign.is_end_tag());
  assert(unit_assign.is_start_tag());
  assert(unit_assign.is_empty_element_tag());
  assert(unit_assign.tag_name() == "TAG_NAME");
  assert(unit_assign.num_attributes() == 1);
  assert(&unit_assign.attribute(0) == &attribute);

  unit.Clear();

  unit.set_type(nwc_toolkit::HtmlUnit::COMMENT_UNIT);
  assert(unit.type() == nwc_toolkit::HtmlUnit::COMMENT_UNIT);
  assert(unit.comment().is_empty());

  unit.set_comment("COMMENT");
  assert(unit.comment() == "COMMENT");

  unit.Clear();

  unit.set_type(nwc_toolkit::HtmlUnit::OTHER_UNIT);
  assert(unit.type() == nwc_toolkit::HtmlUnit::OTHER_UNIT);
  assert(unit.other_content().is_empty());

  unit.set_other_content("OTHER_CONTENT");
  assert(unit.other_content() == "OTHER_CONTENT");

  return 0;
}
