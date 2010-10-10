// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/text-filter.h>

namespace {

void TestCodePoints() {
  nwc_toolkit::String src = "012。！．";
  nwc_toolkit::String character;
  int code_point = 0;
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point));
  assert(code_point == '0');
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point));
  assert(code_point == '1');
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point));
  assert(code_point == '2');
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point));
  assert(code_point == 0x3002);
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point));
  assert(code_point == 0xFF01);
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point));
  assert(code_point == 0xFF0E);
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point) == false);

  src = "あア亜阿한글ÄÆ";
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point));
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point));
  assert(nwc_toolkit::TextFilter::IsHiragana(code_point));
  assert(nwc_toolkit::TextFilter::ExtractCharacter
      (src, &src, &character, &code_point));
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point));
  assert(nwc_toolkit::TextFilter::IsHiragana(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter
      (src, &src, &character, &code_point));
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point));
  assert(nwc_toolkit::TextFilter::IsHiragana(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter
      (src, &src, &character, &code_point));
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point));
  assert(nwc_toolkit::TextFilter::IsHiragana(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter
      (src, &src, &character, &code_point));
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::IsHiragana(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter
      (src, &src, &character, &code_point));
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::IsHiragana(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter
      (src, &src, &character, &code_point));
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::IsHiragana(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter
      (src, &src, &character, &code_point));
  assert(nwc_toolkit::TextFilter::IsNihongo(code_point) == false);
  assert(nwc_toolkit::TextFilter::IsHiragana(code_point) == false);
  assert(nwc_toolkit::TextFilter::ExtractCharacter(
      src, &src, &character, &code_point) == false);
}

void TestDelimitors() {
  nwc_toolkit::String src = "0.1!2?3。4！5．6？7\r8\n9";
  nwc_toolkit::String dest;
  for (int i = 0; i < 10; ++i) {
    char c = '0' + i;
    nwc_toolkit::String sentence(&c, 1);
    assert(nwc_toolkit::TextFilter::ExtractSentence(src, &src, &dest));
    assert(dest == sentence);
  }
  assert(nwc_toolkit::TextFilter::ExtractSentence(src, &src, &dest) == false);
}

void TestFilter() {
  nwc_toolkit::String src = "あいうえお\nカキクケコサシスセソ\n";
  nwc_toolkit::StringBuilder dest;

  nwc_toolkit::TextFilter::Filter(src, &dest);
  assert(dest.is_empty());

  src = "明日の天気は晴れ．明後日は曇り．";
  nwc_toolkit::TextFilter::Filter(src, &dest);
  assert(dest.str() == "明日の天気は晴れ\n明後日は曇り\n");

  src = " \r\n\t明々後日は雨．\r\n";
  nwc_toolkit::TextFilter::Filter(src, &dest);
  assert(dest.str() == "明日の天気は晴れ\n明後日は曇り\n明々後日は雨\n");

  dest.Clear();

  src = "At least 70% of the 文字s must be 日本語文字s.";
  nwc_toolkit::TextFilter::Filter(src, &dest);
  assert(dest.is_empty());

  src = "平仮名ガ少シ必要．";
  nwc_toolkit::TextFilter::Filter(src, &dest);
  assert(dest.is_empty());

  src = "短すぎる．";
  nwc_toolkit::TextFilter::Filter(src, &dest);
  assert(dest.is_empty());

  src = "\xEF\xBB\xBF制御文字は\xEF\xBB\xBF取り除かれる";
  nwc_toolkit::TextFilter::Filter(src, &dest);
  assert(dest.str() == "制御文字は取り除かれる\n");
}

}  // namespace

int main() {
  TestCodePoints();
  TestDelimitors();
  TestFilter();

  return 0;
}
