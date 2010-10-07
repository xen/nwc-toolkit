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

  entry.Clear();

  nwc_toolkit::StringBuilder unicode_body;
  nwc_toolkit::StringBuilder src_encoding;
  nwc_toolkit::StringBuilder content_type;

  entry.set_body("文字コード変換テスト");

  assert(entry.ExtractUnicodeBody(&unicode_body, &src_encoding));
  assert(unicode_body.str() == "文字コード変換テスト");
  assert(src_encoding.str() == "UTF-8");
  assert(entry.ExtractContentType(&content_type) == false);
  assert(content_type.is_empty());

  entry.set_header("Content-Type: text/html; charset=x-sjis");
  entry.set_body("\x95\xB6\x8E\x9A\x83\x52\x81\x5B\x83\x68"
      "\x95\xCF\x8A\xB7\x83\x65\x83\x58\x83\x67");

  assert(entry.ExtractUnicodeBody(&unicode_body, &src_encoding));
  assert(unicode_body.str() == "文字コード変換テスト");
  assert(src_encoding.str() == "CP932");
  assert(entry.ExtractContentType(&content_type));
  assert(content_type.str() == "text/html");

  entry.set_header("Content-Type: Text/Plain; charset=euc-jp");
  entry.set_body("\xCA\xB8\xBB\xFA\xA5\xB3\xA1\xBC\xA5\xC9"
      "\xCA\xD1\xB4\xB9\xA5\xC6\xA5\xB9\xA5\xC8");

  assert(entry.ExtractUnicodeBody(&unicode_body, &src_encoding));
  assert(unicode_body.str() == "文字コード変換テスト");
  assert(src_encoding.str() == "EUC-JP");
  assert(entry.ExtractContentType(&content_type));
  assert(content_type.str() == "text/plain");

  entry.set_header("Content-Type: Application/xhtml+xml; charset=iso-2022-jp");
  entry.set_body("\x1B\x24\x42\x4A\x38\x3B\x7A\x25\x33\x21\x3C\x25\x49"
      "\x4A\x51\x34\x39\x25\x46\x25\x39\x25\x48\x1B\x28\x42");

  assert(entry.ExtractUnicodeBody(&unicode_body, &src_encoding));
  assert(unicode_body.str() == "文字コード変換テスト");
  assert(src_encoding.str() == "ISO-2022-JP");
  assert(entry.ExtractContentType(&content_type));
  assert(content_type.str() == "application/xhtml+xml");

  return 0;
}
