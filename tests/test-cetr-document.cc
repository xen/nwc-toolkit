// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/cetr-document.h>

int main(int argc, char *argv[]) {
  nwc_toolkit::String html_str =
    "<html lang=\"ja\">\n"
    " <head>\n"
    "  <meta charset=\"utf-8\">\n"
    "  <title>タイトル</title>\n"
    " </head>\n"
    " <body>\n"
    "  <script>SCRIPT</script>\n"
    "  <!-- コメント -->\n"
    "  <h1>見出し</h1>\n"
    "  <p>\n"
    "   <a href=\"http://code.google.com/p/nwc-toolkit/\">nwc-toolkit</a> は"
    "<a href=\"http://s-yata.jp/corpus/nwc2010/\">日本語ウェブコーパス</a>を"
    "作成するために開発されたソフトウェアです．"
    "<a href=\"http://s-yata.jp/corpus/nwc2010/htmls/\">HTML アーカイブ</a>"
    "から<a href=\"http://s-yata.jp/corpus/nwc2010/texts/\">テキストアーカイブ"
    "</a>を作成するツールと，分かち書きしたテキストから "
    "<a href=\"http://s-yata.jp/corpus/nwc2010/ngrams/\">N-gram コーパス</a>"
    "を作成するツールで構成されています．サイズが 1TiB を超える HTML "
    "アーカイブを想定して C++ で開発されたツールなので，"
    "正規表現を多用する HTML パーサなどと比べれば，かなり高速に動作します．\n"
    "  </p>\n"
    "  <p>\n"
    "   以下，nwc-toolkit をインストールする方法とツールの"
    "簡単な説明になっています．\n"
    "  </p>\n"
    " </body>\n"
    "</html>\n";
  nwc_toolkit::HtmlDocument html_doc;
  assert(html_doc.Parse(html_str));

  nwc_toolkit::CetrDocument cetr;

  assert(cetr.num_lines() == 0);
  assert(cetr.num_units() == 0);
  assert(cetr.num_points() == 0);

  cetr.Parse(html_doc);

  assert(cetr.num_lines() == 17);
  assert(cetr.num_units() == 71);
  assert(cetr.num_points() == 17);

  cetr.Clear();

  assert(cetr.num_lines() == 0);
  assert(cetr.num_units() == 0);
  assert(cetr.num_points() == 0);

  return 0;
}
