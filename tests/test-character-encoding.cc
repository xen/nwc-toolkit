// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/character-encoding.h>

namespace {

void TestConvert() {
  nwc_toolkit::StringBuilder dest;

  assert(nwc_toolkit::CharacterEncoding::Convert(
      "UTF-8", "日本語文字コード", "UTF-8", &dest) == true);
  assert(dest.str() == "日本語文字コード");

  assert(nwc_toolkit::CharacterEncoding::Convert(
      "UTF-8", "日本語文字コード", "UTF-8", &dest) == true);
  assert(dest.str() == "日本語文字コード日本語文字コード");

  assert(nwc_toolkit::CharacterEncoding::Convert(
      "EUC-JP", "日本語文字コード", "UTF-8", &dest) == false);
  assert(dest.str() == "日本語文字コード日本語文字コード");

  assert(nwc_toolkit::CharacterEncoding::Convert(
      "SHIFT_JIS", "日本語文字コード", "UTF-8", &dest) == false);
  assert(dest.str() == "日本語文字コード日本語文字コード");
}

void TestDetect() {
  nwc_toolkit::StringBuilder encoding;

  assert(nwc_toolkit::CharacterEncoding::DetectFromBOM(
      "XYZ", &encoding) == false);
  assert(encoding.IsEmpty() == true);

  assert(nwc_toolkit::CharacterEncoding::DetectFromBOM(
      "\xEF\xBB\xBF...", &encoding) == true);
  assert(encoding.str() == "UTF-8");

  assert(nwc_toolkit::CharacterEncoding::DetectFromBOM(
      "\xFF\xFE...", &encoding) == true);
  assert(encoding.str() == "UTF-16LE");

  assert(nwc_toolkit::CharacterEncoding::DetectFromBOM(
      "\xFE\xFF...", &encoding) == true);
  assert(encoding.str() == "UTF-16BE");

  assert(nwc_toolkit::CharacterEncoding::DetectFromResponseHeader(
      "Content-Length: 100", &encoding) == false);
  assert(encoding.IsEmpty() == true);

  assert(nwc_toolkit::CharacterEncoding::DetectFromResponseHeader(
      "Content-Type: text/html; charset=utf-8\n", &encoding) == true);
  assert(encoding.str() == "UTF-8");

  assert(nwc_toolkit::CharacterEncoding::DetectFromResponseHeader(
      "Content-Type: text/html; charset=\"x-sjis\"\n", &encoding) == true);
  assert(encoding.str() == "X-SJIS");

  assert(nwc_toolkit::CharacterEncoding::DetectFromHtmlHeader(
      "<html></html>", &encoding) == false);
  assert(encoding.IsEmpty() == true);

  assert(nwc_toolkit::CharacterEncoding::DetectFromHtmlHeader("<html><head>"
      "<meta http-equiv=\"Content-Type\""
      " content=\"text/html; charset=utf-8\">"
      "</head></html>", &encoding) == true);
  assert(encoding.str() == "UTF-8");

  assert(nwc_toolkit::CharacterEncoding::DetectFromXmlHeader(
      "<?xml version=\"1.0\">", &encoding) == false);
  assert(encoding.IsEmpty() == true);

  assert(nwc_toolkit::CharacterEncoding::DetectFromXmlHeader(
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>", &encoding) == true);
  assert(encoding.str() == "UTF-8");
}

}  // namespace

int main() {
  TestConvert();
  TestDetect();

  return 0;
}
