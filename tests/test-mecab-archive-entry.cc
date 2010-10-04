// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/mecab-archive-entry.h>

int main() {
  nwc_toolkit::OutputFile output_file;
  assert(output_file.Open("test-mecab-archive-entry.dat"));
  assert(output_file.Write(
      "おはよう\t感動詞\nござい\t助動詞\nます\t助動詞\nEOS\nEOS\n"));
  assert(output_file.Close());

  nwc_toolkit::MecabArchiveEntry entry;

  assert(entry.num_words() == 0);

  nwc_toolkit::InputFile input_file;
  assert(input_file.Open("test-mecab-archive-entry.dat"));
  assert(entry.Read(&input_file));
  assert(entry.num_words() == 4);
  assert(entry.word(0) == "おはよう\t感動詞");
  assert(entry.word(1) == "ござい\t助動詞");
  assert(entry.word(2) == "ます\t助動詞");
  assert(entry.word(3) == "EOS");
  assert(input_file.Close());

  assert(output_file.Open("test-mecab-archive-entry.dat"));
  assert(entry.Write(&output_file));
  assert(output_file.Close());

  entry.Clear();

  assert(entry.num_words() == 0);

  assert(input_file.Open("test-mecab-archive-entry.dat"));
  assert(entry.Read(&input_file));
  assert(entry.num_words() == 4);
  assert(entry.word(0) == "おはよう\t感動詞");
  assert(entry.word(1) == "ござい\t助動詞");
  assert(entry.word(2) == "ます\t助動詞");
  assert(entry.word(3) == "EOS");
  assert(entry.Read(&input_file) == false);
  assert(input_file.Close());

  return 0;
}
