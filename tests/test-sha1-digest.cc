// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/sha1-digest.h>

namespace {

void TestShortPair() {
  nwc_toolkit::String src = "This is a pen.\n";
  nwc_toolkit::String dest = "d4d46e90cd7b64248fbbbb458bea0568cbea61d4";

  nwc_toolkit::Sha1Digest digest;
  digest.Append(src);
  assert(digest.length() == src.length());
  digest.Finish();
  assert(digest.hex_digest() == dest);

  digest.Clear();

  assert(digest.hex_digest() != dest);

  digest.Append(src);
  assert(digest.length() == src.length());
  digest.Finish();
  assert(digest.hex_digest() == dest);
}

void TestLongPair() {
  nwc_toolkit::String src = "nwc-toolkit は日本語ウェブコーパスを扱うために開発されたソフトウェアです．日本語で記述された HTML 文書からテキストを抽出したり，Unicode 正規化や簡単なフィルタリングを施したりできるほか，分かち書きしたテキストから N-gram コーパスを作成することが可能です．";
  nwc_toolkit::String dest = "2199e67ee69b6f5e165c5585d23cbded09769893";

  nwc_toolkit::Sha1Digest digest;
  digest.Append(src);
  assert(digest.length() == src.length());
  digest.Finish();
  assert(digest.hex_digest() == dest);

  digest.Clear();

  assert(digest.hex_digest() != dest);

  digest.Append(src);
  assert(digest.length() == src.length());
  digest.Finish();
  assert(digest.hex_digest() == dest);

}

}  // namespace

int main() {
  TestShortPair();
  TestLongPair();

  return 0;
}
