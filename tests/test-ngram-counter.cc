// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>

#include <nwc-toolkit/ngram-counter.h>

namespace {

void TestSettings() {
  nwc_toolkit::NgramCounter ngram_counter;

  assert(ngram_counter.input_format() ==
      nwc_toolkit::NgramCounter::DEFAULT_FORMAT);
  assert(ngram_counter.with_boundary_count() == false);
  assert(ngram_counter.with_result_sort() == false);
  assert(ngram_counter.memory_limit() ==
      nwc_toolkit::NgramCounter::DEFAULT_MEMORY_LIMIT);
  assert(ngram_counter.max_ngram_length() ==
      nwc_toolkit::NgramCounter::DEFAULT_MAX_NGRAM_LENGTH);

  assert(ngram_counter.token_count() == 0);
  assert(ngram_counter.sentence_count() == 0);

  ngram_counter.set_input_format(
      nwc_toolkit::NgramCounter::CHASEN_FORMAT);
  ngram_counter.set_with_boundary_count(true);
  ngram_counter.set_with_result_sort(true);
  ngram_counter.Reset(1, 1);

  assert(ngram_counter.input_format() ==
      nwc_toolkit::NgramCounter::CHASEN_FORMAT);
  assert(ngram_counter.with_boundary_count() == true);
  assert(ngram_counter.with_result_sort() == true);
  assert(ngram_counter.memory_limit() ==
      nwc_toolkit::NgramCounter::MIN_MEMORY_LIMIT);
  assert(ngram_counter.max_ngram_length() ==
      nwc_toolkit::NgramCounter::MIN_MAX_NGRAM_LENGTH);
}

void TestWakati() {
  nwc_toolkit::OutputFile output_file;
  assert(output_file.Open("test-ngram-counter.wakati"));
  assert(output_file.Write("a b c\n\na b c\n"));
  assert(output_file.Close());

  nwc_toolkit::InputFile input_file;
  assert(input_file.Open("test-ngram-counter.wakati"));

  nwc_toolkit::NgramCounter ngram_counter;
  ngram_counter.set_input_format(
      nwc_toolkit::NgramCounter::WAKATI_FORMAT);
  ngram_counter.Reset(5, 1);

  assert(ngram_counter.Count(&input_file));
  assert(ngram_counter.Count(&input_file));
  assert(ngram_counter.Count(&input_file) == false);

  assert(ngram_counter.token_count() == 6);
  assert(ngram_counter.sentence_count() == 2);

  assert(input_file.Close());

  assert(output_file.Open("test-ngram-counter.wakati.out"));
  assert(ngram_counter.Flush(&output_file));
  assert(output_file.Close());

  assert(input_file.Open("test-ngram-counter.wakati.out"));
  nwc_toolkit::String line;
  for (int i = 0; i < 6; ++i) {
    assert(input_file.ReadLine(&line));
    assert(line.EndsWith("\n"));
    line = line.SubString(0, line.length() - 1);
    nwc_toolkit::String freq(line.FindLastOf('\t').end(), line.end());
    assert(freq == "2");
  }
  assert(input_file.ReadLine(&line) == false);
}

void TestMecab() {
  nwc_toolkit::OutputFile output_file;
  assert(output_file.Open("test-ngram-counter.mecab.gz"));
  assert(output_file.Write("a\nb\nc\nEOS\nEOS\na\nb\nc\nEOS\n"));
  assert(output_file.Close());

  nwc_toolkit::InputFile input_file;
  assert(input_file.Open("test-ngram-counter.mecab.gz"));

  nwc_toolkit::NgramCounter ngram_counter;
  ngram_counter.set_input_format(
      nwc_toolkit::NgramCounter::MECAB_FORMAT);
  ngram_counter.set_with_result_sort(true);
  ngram_counter.Reset(5, 1);

  assert(ngram_counter.Count(&input_file));
  assert(ngram_counter.Count(&input_file));
  assert(ngram_counter.Count(&input_file) == false);

  assert(ngram_counter.token_count() == 6);
  assert(ngram_counter.sentence_count() == 2);

  assert(input_file.Close());

  assert(output_file.Open("test-ngram-counter.mecab.out.gz"));
  assert(ngram_counter.Flush(&output_file));
  assert(output_file.Close());

  assert(input_file.Open("test-ngram-counter.mecab.out.gz"));
  nwc_toolkit::StringBuilder last_line;
  nwc_toolkit::String line;
  for (int i = 0; i < 6; ++i) {
    assert(input_file.ReadLine(&line));
    assert(line.EndsWith("\n"));
    line = line.SubString(0, line.length() - 1);
    nwc_toolkit::String freq(line.FindLastOf('\t').end(), line.end());
    assert(freq == "2");

    assert(line > last_line.str());
    last_line = line;
  }
  assert(input_file.ReadLine(&line) == false);
}

void TestChasen() {
  nwc_toolkit::OutputFile output_file;
  assert(output_file.Open("test-ngram-counter.chasen.gz"));
  assert(output_file.Write("a\nb\nc\nEOS\nEOS\na\nb\nc\nEOS\n"));
  assert(output_file.Close());

  nwc_toolkit::InputFile input_file;
  assert(input_file.Open("test-ngram-counter.chasen.gz"));

  nwc_toolkit::NgramCounter ngram_counter;
  ngram_counter.set_input_format(
      nwc_toolkit::NgramCounter::CHASEN_FORMAT);
  ngram_counter.set_with_boundary_count(true);
  ngram_counter.set_with_result_sort(true);
  ngram_counter.Reset(5, 1);

  assert(ngram_counter.Count(&input_file));
  assert(ngram_counter.Count(&input_file));
  assert(ngram_counter.Count(&input_file) == false);

  assert(ngram_counter.token_count() == 10);
  assert(ngram_counter.sentence_count() == 2);

  assert(input_file.Close());

  assert(output_file.Open("test-ngram-counter.chasen.out.xz"));
  assert(ngram_counter.Flush(&output_file));
  assert(output_file.Close());

  assert(input_file.Open("test-ngram-counter.chasen.out.xz"));
  nwc_toolkit::StringBuilder last_line;
  nwc_toolkit::String line;
  for (int i = 0; i < 15; ++i) {
    assert(input_file.ReadLine(&line));
    assert(line.EndsWith("\n"));
    line = line.SubString(0, line.length() - 1);
    nwc_toolkit::String freq(line.FindLastOf('\t').end(), line.end());
    assert(freq == "2");

    assert(line > last_line.str());
    last_line = line;
  }
  assert(input_file.ReadLine(&line) == false);
}

}  // namespace

int main() {
  TestSettings();

  TestWakati();
  TestMecab();
  TestChasen();

  return 0;
}
