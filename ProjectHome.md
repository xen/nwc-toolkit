Project URL: http://code.google.com/p/nwc-toolkit/

nwc-toolkit は[日本語ウェブコーパス](http://s-yata.jp/corpus/nwc2010/)を作成するために開発されたソフトウェアです．[HTML アーカイブ](http://s-yata.jp/corpus/nwc2010/htmls/)から[テキストアーカイブ](http://s-yata.jp/corpus/nwc2010/texts/)を作成するツールと，分かち書きしたテキストから [N-gram コーパス](http://s-yata.jp/corpus/nwc2010/ngrams/)を作成するツールで構成されています．サイズが 1TiB を超える HTML アーカイブを想定して C++ で開発されたツールなので，正規表現を多用する HTML パーサなどと比べれば，かなり高速に動作します．

## ドキュメント ##

  * [nwc-toolkit（日本語ウェブコーパス用ツールキット）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/index.html)
    * テキストアーカイブの作成
      * [nwc-toolkit-text-extractor（テキスト抽出ツール）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/tools/text-extractor.html)
        * [テキスト抽出ツールのウェブサービス](http://s-yata.jp/apps/nwc-toolkit/text-extractor)
      * [nwc-toolkit-html-parser（HTML 解析ツール）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/tools/html-parser.html)
      * [nwc-toolkit-unicode-normalizer（Unicode 正規化ツール）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/tools/unicode-normalizer.html)
      * [nwc-toolkit-text-filter（簡易文抽出ツール）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/tools/text-filter.html)
    * N-gram コーパスの作成
      * [nwc-toolkit-ngram-counter（N-gram 頻度計数ツール）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/tools/ngram-counter.html)
      * [nwc-toolkit-ngram-merger（N-gram コーパス統合ツール）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/tools/ngram-merger.html)
    * その他
      * [nwc-toolkit-hash-calculator（ハッシュ値計算ツール）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/tools/hash-calculator.html)
      * [nwc-toolkit-duplicate-detector（重複検出ツール）](http://nwc-toolkit.googlecode.com/svn/trunk/docs/tools/duplicate-detector.html)