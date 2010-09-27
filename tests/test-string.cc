// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/string.h>

namespace {

void TestConstructors() {
  nwc_toolkit::String str_0;
  assert(str_0.IsEmpty());

  nwc_toolkit::String str_1("abc");
  assert(str_1 == "abc");

  nwc_toolkit::String str_2("abc", 2);
  assert(str_2 == "ab");

  nwc_toolkit::String str_3("abc", 4);
  assert(str_3.Compare("abc", 4) == 0);

  char buf[] = "0123456789";

  nwc_toolkit::String str_4(buf + 1, 3);
  assert(str_4 == "123");

  nwc_toolkit::String str_5(buf + 2, buf + 5);
  assert(str_5 == "234");

  str_5.Clear();
  assert(str_5.IsEmpty());
}

void TestOperators() {
  nwc_toolkit::String empty;
  assert(empty == "");
  assert(empty != "abc");

  nwc_toolkit::String abc("abc");
  assert(abc != "");
  assert(abc == "abc");
  assert(abc != "abcd");

  nwc_toolkit::String xyz("xyz");
  assert(xyz.IsEmpty() == false);
  assert(xyz == "xyz");

  assert(abc <= "abc");
  assert(abc <= "xyz");
  assert(abc < "xyz");

  assert(xyz >= "xyz");
  assert(xyz >= "abc");
  assert(xyz >= "");

  assert(empty == empty);
  assert(abc == abc);
  assert(xyz == xyz);

  assert(empty < abc);
  assert(abc < xyz);

  assert(xyz > abc);
  assert(abc > empty);

  assert(empty <= empty);
  assert(empty <= abc);
  assert(empty <= xyz);

  assert(abc >= abc);
  assert(xyz >= abc);

  assert((abc = xyz) == xyz);
  assert(abc == xyz);
}

void TestChanges() {
  char buf[] = "0123456789";
  nwc_toolkit::String str(buf);

  assert(str.set_begin(buf + 3) == "3456789");
  assert(str.begin() == buf + 3);
  assert(str.end() == buf + 10);

  assert(str.set_end(buf + 7) == "3456");
  assert(str.begin() == buf + 3);
  assert(str.end() == buf + 7);

  assert(str.Assign(buf + 2, 3) == "234");
  assert(str.Assign(buf + 4, buf + 7) == "456");

  assert(str.Assign(str) == "456");
  assert(str.Assign(buf) == buf);
  assert(str == "0123456789");

  assert(str.SubString(5) == "56789");
  assert(str.SubString(3, 3) == "345");
}

void TestContains() {
  nwc_toolkit::String str("ABCdef");

  assert(str.Contains('A') == true);
  assert(str.Contains('X') == false);

  assert(str.Contains("abcDEF") == false);
  assert(str.Contains("xyzfgh") == true);

  assert(str.Contains("xyzABC", 3) == false);
  assert(str.Contains("xyzABC", 4) == true);

  static const nwc_toolkit::CharTable ABC_TABLE("ABC");
  assert(str.Contains(ABC_TABLE) == true);

  static const nwc_toolkit::CharTable DEF_TABLE("DEF");
  assert(str.Contains(DEF_TABLE) == false);
}

void TestCompare() {
  nwc_toolkit::String empty;
  nwc_toolkit::String abc("abc");
  nwc_toolkit::String ABC("ABC");
  nwc_toolkit::String AhatC("A^C");

  assert(empty.Compare(empty) == 0);
  assert(empty.Compare(abc) < 0);

  assert(abc.Compare(empty) > 0);
  assert(abc.Compare(abc) == 0);
  assert(abc.Compare(ABC) > 0);

  assert(ABC.Compare(abc) < 0);

  assert(abc.Compare(ABC, nwc_toolkit::ToUpper()) == 0);
  assert(abc.Compare(ABC, nwc_toolkit::ToLower()) == 0);

  assert(abc.Compare(AhatC, nwc_toolkit::ToUpper()) < 0);
  assert(abc.Compare(AhatC, nwc_toolkit::ToLower()) > 0);
}

void TestWith() {
  nwc_toolkit::String abcdef("abcde");

  assert(abcdef.StartsWith("abc"));
  assert(abcdef.StartsWith("abcde012") == false);
  assert(abcdef.StartsWith("bcd") == false);

  assert(abcdef.StartsWith("ABC", nwc_toolkit::ToUpper()));
  assert(abcdef.StartsWith("ABC", nwc_toolkit::ToLower()));

  assert(abcdef.EndsWith("cde"));
  assert(abcdef.EndsWith("012abcde") == false);
  assert(abcdef.EndsWith("bcd") == false);

  assert(abcdef.EndsWith("CDE", nwc_toolkit::ToUpper()));
  assert(abcdef.EndsWith("CDE", nwc_toolkit::ToLower()));
}

void TestFind() {
  nwc_toolkit::String str("0123456ABC0123456DEF");

  assert(str.Find("ABC") == "ABC");
  assert(str.Find("abc", nwc_toolkit::ToUpper()) == "ABC");
  assert(str.Find("abc", nwc_toolkit::ToLower()) == "ABC");

  assert(str.Find("XYZ") == "");
  assert(str.Find("XYZ").begin() == str.end());

  assert(str.Find("") == "");
  assert(str.Find("").end() == str.begin());

  assert(str.Find("BCD", 2) == "BC");
  assert(str.Find("bcd", 2, nwc_toolkit::ToUpper()) == "BC");
}

void TestFindOf() {
  nwc_toolkit::String str("0123456ABC0123456DEF");

  assert(str.FindFirstOf('B') == "B");
  assert(str.FindFirstOf('X') == "");
  assert(str.FindFirstOf(nwc_toolkit::IsAlpha()) == "A");

  assert(str.FindFirstNotOf("0123456789") == "A");
  assert(str.FindFirstNotOf("012XYZ") == "3");
  assert(str.FindFirstNotOf(nwc_toolkit::IsAlnum()) == "");

  assert(str.FindLastOf('B') == "B");
  assert(str.FindLastOf('X') == "");
  assert(str.FindLastOf(nwc_toolkit::IsAlpha()) == "F");

  assert(str.FindLastNotOf("0123456789DEF") == "C");
  assert(str.FindLastNotOf("") == "F");
  assert(str.FindLastNotOf(nwc_toolkit::IsAlnum()) == "");
}

void TestStrip() {
  nwc_toolkit::String str("  \tABC\n  ");

  assert(str.Strip() == "ABC");
  assert(str.StripLeft() == "ABC\n  ");
  assert(str.StripRight() == "  \tABC");

  nwc_toolkit::String abc("ABC123XYZ");

  assert(abc.Strip(nwc_toolkit::IsUpper()) == "123");
  assert(abc.Strip(nwc_toolkit::IsAlnum()) == "");
}

}  // namespace

int main() {
  TestConstructors();
  TestOperators();
  TestChanges();
  TestContains();
  TestCompare();
  TestWith();
  TestFind();
  TestFindOf();
  TestStrip();

  return 0;
}
