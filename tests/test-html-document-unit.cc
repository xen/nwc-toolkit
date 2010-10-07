// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/html-document-unit.h>

int main() {
  nwc_toolkit::HtmlDocumentUnit unit;

  assert(unit.type() == nwc_toolkit::HtmlDocumentUnit::UNDEFINED_UNIT);
  assert(unit.src().is_empty());

  unit.set_src("SRC");
  assert(unit.src() == "SRC");

  unit.Clear();

  assert(unit.src().is_empty());

  unit.set_type(nwc_toolkit::HtmlDocumentUnit::TEXT_UNIT);
  assert(unit.type() == nwc_toolkit::HtmlDocumentUnit::TEXT_UNIT);
  assert(unit.is_cdata_section() == false);
  assert(unit.text_content().is_empty());

  unit.set_cdata_section_flag();
  assert(unit.is_cdata_section());

  unit.set_text_content("TEXT_CONTENT");
  assert(unit.text_content() == "TEXT_CONTENT");

  unit.Clear();

  unit.set_type(nwc_toolkit::HtmlDocumentUnit::TAG_UNIT);
  assert(unit.type() == nwc_toolkit::HtmlDocumentUnit::TAG_UNIT);
  assert(unit.is_start_tag() == false);
  assert(unit.is_end_tag() == false);
  assert(unit.is_empty_element_tag() == false);
  assert(unit.tag_name().is_empty());

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

  unit.Clear();

  unit.set_type(nwc_toolkit::HtmlDocumentUnit::COMMENT_UNIT);
  assert(unit.type() == nwc_toolkit::HtmlDocumentUnit::COMMENT_UNIT);
  assert(unit.comment().is_empty());

  unit.set_comment("COMMENT");
  assert(unit.comment() == "COMMENT");

  unit.Clear();

  unit.set_type(nwc_toolkit::HtmlDocumentUnit::OTHER_UNIT);
  assert(unit.type() == nwc_toolkit::HtmlDocumentUnit::OTHER_UNIT);
  assert(unit.other_content().is_empty());

  unit.set_other_content("OTHER_CONTENT");
  assert(unit.other_content() == "OTHER_CONTENT");

  return 0;
}
