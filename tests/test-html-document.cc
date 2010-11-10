// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/html-document.h>

namespace {

void TestPlainText() {
  nwc_toolkit::HtmlDocument document;

  assert(document.body().is_empty());
  assert(document.src_encoding().is_empty());
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::UNDEFINED_MODE);
  assert(document.num_units() == 0);

  nwc_toolkit::HtmlArchiveEntry entry;
  entry.set_header("Content-Type: text/plain; charset=utf-8\n");
  entry.set_body("1 &lt; 2");

  assert(document.Parse(entry));

  assert(document.body() == "1 &lt; 2");
  assert(document.src_encoding() == "UTF-8");
  assert(document.content_type() == "text/plain");
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::PLAIN_TEXT_MODE);

  assert(document.num_units() == 1);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(0).src() == "1 &lt; 2");
  assert(document.unit(0).is_cdata_section() == false);
  assert(document.unit(0).text_content() == "1 &lt; 2");

  nwc_toolkit::StringBuilder text;
  document.ExtractText(&text);
  assert(text.str() == "1 &lt; 2\n");

  document.Clear();

  assert(document.body().is_empty());
  assert(document.src_encoding().is_empty());
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::UNDEFINED_MODE);
  assert(document.num_units() == 0);

  document.ExtractText(&text);
  assert(text.str() == "1 &lt; 2\n");

  entry.Clear();

  entry.set_url("http://www.example.com/robots.txt");
  entry.set_body("2 &gt; 1");

  assert(document.Parse(entry));

  assert(document.body() == "2 &gt; 1");
  assert(document.src_encoding() == "CP932");
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::PLAIN_TEXT_MODE);

  assert(document.num_units() == 1);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(0).src() == "2 &gt; 1");
  assert(document.unit(0).is_cdata_section() == false);
  assert(document.unit(0).text_content() == "2 &gt; 1");

  document.ExtractText(&text);
  assert(text.str() == "1 &lt; 2\n2 &gt; 1\n");
}

void TestXml() {
  nwc_toolkit::HtmlDocument document;

  nwc_toolkit::HtmlArchiveEntry entry;
  entry.set_body("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<html Lang=\"J&#65;\">&lt;Text&gt;<![CDATA[&lt;Abc&gt;]]>"
      "<BR/></html><!-- comment -->");

  assert(document.Parse(entry));

  assert(document.body() == "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<html Lang=\"J&#65;\">&lt;Text&gt;<![CDATA[&lt;Abc&gt;]]>"
      "<BR/></html><!-- comment -->");
  assert(document.src_encoding() == "UTF-8");
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::XML_MODE);

  assert(document.num_units() == 7);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::OTHER_UNIT);
  assert(document.unit(0).src() ==
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  assert(document.unit(0).other_content() ==
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

  assert(document.unit(1).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(1).src() == "<html Lang=\"J&#65;\">");
  assert(document.unit(1).is_start_tag());
  assert(document.unit(1).is_end_tag() == false);
  assert(document.unit(1).is_empty_element_tag() == false);
  assert(document.unit(1).tag_name() == "html");
  assert(document.unit(1).num_attributes() == 1);
  assert(document.unit(1).attribute(0).name() == "lang");
  assert(document.unit(1).attribute(0).value() == "JA");

  assert(document.unit(2).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(2).src() == "&lt;Text&gt;");
  assert(document.unit(2).is_cdata_section() == false);
  assert(document.unit(2).text_content() == "<Text>");

  assert(document.unit(3).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(3).src() == "<![CDATA[&lt;Abc&gt;]]>");
  assert(document.unit(3).is_cdata_section());
  assert(document.unit(3).text_content() == "&lt;Abc&gt;");

  assert(document.unit(4).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(4).is_start_tag());
  assert(document.unit(4).is_end_tag() == false);
  assert(document.unit(4).is_empty_element_tag());
  assert(document.unit(4).src() == "<BR/>");
  assert(document.unit(4).tag_name() == "br");
  assert(document.unit(4).num_attributes() == 0);

  assert(document.unit(5).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(5).is_start_tag() == false);
  assert(document.unit(5).is_end_tag());
  assert(document.unit(5).is_empty_element_tag() == false);
  assert(document.unit(5).src() == "</html>");
  assert(document.unit(5).tag_name() == "html");
  assert(document.unit(5).num_attributes() == 0);

  assert(document.unit(6).type() ==
      nwc_toolkit::HtmlUnit::COMMENT_UNIT);
  assert(document.unit(6).src() == "<!-- comment -->");
  assert(document.unit(6).text_content() == " comment ");

  nwc_toolkit::StringBuilder text;
  document.ExtractText(&text);
  assert(text.str() == "<Text>&lt;Abc&gt;\n");
}

void TestSimpleHtmlDocuments() {
  nwc_toolkit::HtmlDocument document;

  nwc_toolkit::HtmlArchiveEntry entry;
  entry.set_body("<html Lang=\"J&#65;\">"
      "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
      "&lt;Text&gt;<![CDATA[&lt;Abc&gt;]]>"
      "<BR/></html><!-- comment -->");

  assert(document.Parse(entry));

  assert(document.body() == "<html Lang=\"J&#65;\">"
      "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
      "&lt;Text&gt;<![CDATA[&lt;Abc&gt;]]>"
      "<BR/></html><!-- comment -->");

  assert(document.src_encoding() == "UTF-8");
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::HTML_MODE);

  assert(document.num_units() == 7);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(0).src() == "<html Lang=\"J&#65;\">");
  assert(document.unit(0).is_start_tag());
  assert(document.unit(0).is_end_tag() == false);
  assert(document.unit(0).is_empty_element_tag() == false);
  assert(document.unit(0).tag_name() == "html");
  assert(document.unit(0).num_attributes() == 1);
  assert(document.unit(0).attribute(0).name() == "lang");
  assert(document.unit(0).attribute(0).value() == "JA");

  assert(document.unit(1).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(1).src() == "<meta http-equiv=\"Content-Type\" "
      "content=\"text/html; charset=UTF-8\">");
  assert(document.unit(1).is_start_tag());
  assert(document.unit(1).is_end_tag() == false);
  assert(document.unit(1).is_empty_element_tag() == false);
  assert(document.unit(1).tag_name() == "meta");
  assert(document.unit(1).num_attributes() == 2);
  assert(document.unit(1).attribute(0).name() == "http-equiv");
  assert(document.unit(1).attribute(0).value() == "Content-Type");
  assert(document.unit(1).attribute(1).name() == "content");
  assert(document.unit(1).attribute(1).value() == "text/html; charset=UTF-8");

  assert(document.unit(2).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(2).src() == "&lt;Text&gt;");
  assert(document.unit(2).is_cdata_section() == false);
  assert(document.unit(2).text_content() == "<Text>");

  assert(document.unit(3).type() == nwc_toolkit::HtmlUnit::OTHER_UNIT);
  assert(document.unit(3).src() == "<![CDATA[&lt;Abc&gt;]]>");
  assert(document.unit(3).text_content() == "<![CDATA[&lt;Abc&gt;]]>");

  assert(document.unit(4).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(4).is_start_tag());
  assert(document.unit(4).is_end_tag() == false);
  assert(document.unit(4).is_empty_element_tag());
  assert(document.unit(4).src() == "<BR/>");
  assert(document.unit(4).tag_name() == "br");
  assert(document.unit(4).num_attributes() == 0);

  assert(document.unit(5).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(5).is_start_tag() == false);
  assert(document.unit(5).is_end_tag());
  assert(document.unit(5).is_empty_element_tag() == false);
  assert(document.unit(5).src() == "</html>");
  assert(document.unit(5).tag_name() == "html");
  assert(document.unit(5).num_attributes() == 0);

  assert(document.unit(6).type() ==
      nwc_toolkit::HtmlUnit::COMMENT_UNIT);
  assert(document.unit(6).src() == "<!-- comment -->");
  assert(document.unit(6).text_content() == " comment ");

  nwc_toolkit::StringBuilder text;
  document.ExtractText(&text);
  assert(text.str() == "<Text>\n");

  entry.Clear();
  entry.set_body("<meta charset=\"euc-jp\">");

  assert(document.Parse(entry));
  assert(document.src_encoding() == "EUC-JP");
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::HTML_MODE);

  assert(document.num_units() == 1);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(0).src() == "<meta charset=\"euc-jp\">");
  assert(document.unit(0).is_start_tag());
  assert(document.unit(0).is_end_tag() == false);
  assert(document.unit(0).is_empty_element_tag() == false);
  assert(document.unit(0).tag_name() == "meta");
  assert(document.unit(0).num_attributes() == 1);
  assert(document.unit(0).attribute(0).name() == "charset");
  assert(document.unit(0).attribute(0).value() == "euc-jp");

  document.ExtractText(&text);
  assert(text.str() == "<Text>\n");
}

void TestComplexHtmlDocuments() {
  nwc_toolkit::HtmlDocument document;

  nwc_toolkit::HtmlArchiveEntry entry;
  entry.set_body("<script type=\"text/javascript\">"
      "<b></b><!--\n// -->\n</script>");

  assert(document.Parse(entry));

  assert(document.src_encoding() == "CP932");
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::HTML_MODE);

  assert(document.num_units() == 3);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(0).src() == "<script type=\"text/javascript\">");
  assert(document.unit(0).is_start_tag());
  assert(document.unit(0).is_end_tag() == false);
  assert(document.unit(0).is_empty_element_tag() == false);
  assert(document.unit(0).tag_name() == "script");
  assert(document.unit(0).num_attributes() == 1);
  assert(document.unit(0).attribute(0).name() == "type");
  assert(document.unit(0).attribute(0).value() == "text/javascript");

  assert(document.unit(1).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(1).src() == "<b></b><!--\n// -->\n");
  assert(document.unit(1).is_cdata_section() == false);
  assert(document.unit(1).text_content() == "<b></b><!--\n// -->\n");

  assert(document.unit(2).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(2).src() == "</script>");
  assert(document.unit(2).is_start_tag() == false);
  assert(document.unit(2).is_end_tag());
  assert(document.unit(2).is_empty_element_tag() == false);
  assert(document.unit(2).tag_name() == "script");
  assert(document.unit(2).num_attributes() == 0);

  nwc_toolkit::StringBuilder text;
  document.ExtractText(&text);
  assert(text.str().is_empty());

  entry.Clear();
  entry.set_body("<style type=\"text/css\"><!--\n-->\n</style>");

  assert(document.Parse(entry));

  assert(document.src_encoding() == "CP932");
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::HTML_MODE);

  assert(document.num_units() == 3);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(0).src() == "<style type=\"text/css\">");
  assert(document.unit(0).is_start_tag());
  assert(document.unit(0).is_end_tag() == false);
  assert(document.unit(0).is_empty_element_tag() == false);
  assert(document.unit(0).tag_name() == "style");
  assert(document.unit(0).num_attributes() == 1);
  assert(document.unit(0).attribute(0).name() == "type");
  assert(document.unit(0).attribute(0).value() == "text/css");

  assert(document.unit(1).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(1).src() == "<!--\n-->\n");
  assert(document.unit(1).is_cdata_section() == false);
  assert(document.unit(1).text_content() == "<!--\n-->\n");

  assert(document.unit(2).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(2).src() == "</style>");
  assert(document.unit(2).is_start_tag() == false);
  assert(document.unit(2).is_end_tag());
  assert(document.unit(2).is_empty_element_tag() == false);
  assert(document.unit(2).tag_name() == "style");
  assert(document.unit(2).num_attributes() == 0);

  document.ExtractText(&text);
  assert(text.str().is_empty());

  entry.Clear();
  entry.set_body("<plaintext> \n \t</plaintext>\r\n");

  assert(document.Parse(entry));

  assert(document.src_encoding() == "CP932");
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::HTML_MODE);

  assert(document.num_units() == 2);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(0).src() == "<plaintext>");
  assert(document.unit(0).is_start_tag());
  assert(document.unit(0).is_end_tag() == false);
  assert(document.unit(0).is_empty_element_tag() == false);
  assert(document.unit(0).tag_name() == "plaintext");
  assert(document.unit(0).num_attributes() == 0);

  assert(document.unit(1).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(1).src() == " \n \t</plaintext>\r\n");
  assert(document.unit(1).is_cdata_section() == false);
  assert(document.unit(1).text_content() == " \n \t</plaintext>\r\n");

  document.ExtractText(&text);
  assert(text.str() == "</plaintext>\n");

  entry.Clear();
  entry.set_body("<TEXTAREA><A>&lt;/A&gt;</TEXTAREA>");

  assert(document.Parse(entry));

  assert(document.src_encoding() == "CP932");
  assert(document.content_type().is_empty());
  assert(document.parser_mode() == nwc_toolkit::HtmlDocument::HTML_MODE);

  assert(document.num_units() == 3);

  assert(document.unit(0).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(0).src() == "<TEXTAREA>");
  assert(document.unit(0).is_start_tag());
  assert(document.unit(0).is_end_tag() == false);
  assert(document.unit(0).is_empty_element_tag() == false);
  assert(document.unit(0).tag_name() == "textarea");
  assert(document.unit(0).num_attributes() == 0);

  assert(document.unit(1).type() == nwc_toolkit::HtmlUnit::TEXT_UNIT);
  assert(document.unit(1).src() == "<A>&lt;/A&gt;");
  assert(document.unit(1).is_cdata_section() == false);
  assert(document.unit(1).text_content() == "<A></A>");

  assert(document.unit(2).type() == nwc_toolkit::HtmlUnit::TAG_UNIT);
  assert(document.unit(2).src() == "</TEXTAREA>");
  assert(document.unit(2).is_start_tag() == false);
  assert(document.unit(2).is_end_tag());
  assert(document.unit(2).is_empty_element_tag() == false);
  assert(document.unit(2).tag_name() == "textarea");
  assert(document.unit(2).num_attributes() == 0);

  text.Clear();
  document.ExtractText(&text);
  assert(text.str() == "<A></A>\n");
}

}  // namespace

int main() {
  TestPlainText();
  TestXml();
  TestSimpleHtmlDocuments();
  TestComplexHtmlDocuments();

  return 0;
}
