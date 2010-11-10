// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <tr1/random>
#include <vector>

#include <nwc-toolkit/input-file.h>
#include <nwc-toolkit/output-file.h>
#include <nwc-toolkit/string-builder.h>
#include <nwc-toolkit/string-pool.h>

namespace {

std::tr1::mt19937 mt_rand(static_cast<unsigned int>(time(NULL)));

void TestOutputText(const char *path, const nwc_toolkit::String &text) {
  nwc_toolkit::OutputFile file;
  assert(file.is_open() == false);

  assert(file.Open(path, 0, nwc_toolkit::Coder::BEST_SPEED_PRESET));
  assert(file.is_open());

  assert(file.Write(text));

  assert(file.Close());
  assert(file.is_open() == false);
}

void TestInputText(const char *path, const nwc_toolkit::String &text) {
  nwc_toolkit::InputFile file;
  assert(file.is_open() == false);

  assert(file.Open(path));
  assert(file.is_open());

  nwc_toolkit::String data;

  assert(file.Read(text.length(), &data));
  assert(data == text);
  assert(file.Read(1, &data) == false);
  assert(data.is_empty());

  assert(file.Close());
  assert(file.is_open() == false);

  assert(file.Open(path));
  assert(file.is_open());

  enum { MAX_IO_SIZE = (1 << 12) - 1 };

  nwc_toolkit::StringBuilder text_buf;
  std::size_t avail = text.length();
  while (avail > 0) {
    std::size_t io_size = mt_rand() % (MAX_IO_SIZE + 1);
    if (io_size > avail) {
      io_size = avail;
    }
    nwc_toolkit::String chunk;
    assert(file.Read(io_size, &chunk));
    assert(chunk.length() == io_size);
    assert(chunk == text.SubString(text.length() - avail, io_size));
    text_buf.Append(chunk);
    avail -= io_size;
  }
  assert(file.Read(1, &data) == false);
  assert(data.is_empty());

  assert(text_buf.str() == text);

  assert(file.Close());
  assert(file.is_open() == false);
}

void TestOutputLines(const char *path,
    const std::vector<nwc_toolkit::String> &lines) {
  nwc_toolkit::OutputFile file;
  assert(file.is_open() == false);

  assert(file.Open(path, 0, nwc_toolkit::Coder::BEST_SPEED_PRESET));
  assert(file.is_open());

  for (std::size_t i = 0; i < lines.size(); ++i) {
    assert(file.Write(lines[i]));
    assert(file.Write("\n"));
  }

  assert(file.Close());
  assert(file.is_open() == false);
}

void TestInputLines(const char *path,
    const std::vector<nwc_toolkit::String> &lines) {
  nwc_toolkit::InputFile file;
  assert(file.Open(path));
  assert(file.is_open());

  nwc_toolkit::String line;
  for (std::size_t i = 0; i < lines.size(); ++i) {
    assert(file.ReadLine(&line));
    assert(line.EndsWith("\n"));
    assert(line.StripRight() == lines[i]);
  }
  assert(file.ReadLine(&line) == false);
}

void TestFileIO(const char *path, const nwc_toolkit::String &text,
    const std::vector<nwc_toolkit::String> &lines) {
  std::cerr << "text: ";
  std::cerr << "output: ";
  TestOutputText(path, text);
  std::cerr << "ok, input: ";
  TestInputText(path, text);
  std::cerr << "ok";

  std::cerr << ", lines: ";
  std::cerr << "output: ";
  TestOutputLines(path, lines);
  std::cerr << "ok, input: ";
  TestInputLines(path, lines);
  std::cerr << "ok" << std::endl;
}

}  // namespace

int main() {
  enum { MAX_LINE_LENGTH = (1 << 8) - 1, NUM_LINES = 1 << 12 };

  nwc_toolkit::StringBuilder line_buf;
  nwc_toolkit::StringPool line_pool;
  std::vector<nwc_toolkit::String> lines;
  nwc_toolkit::StringBuilder text_buf;

  for (std::size_t i = 0; i < NUM_LINES; ++i) {
    line_buf.Clear();
    std::size_t line_length = mt_rand() % (MAX_LINE_LENGTH + 1);
    for (std::size_t j = 0; j < line_length; ++j) {
      line_buf.Append('A' + (mt_rand() % 26));
    }
    nwc_toolkit::String line = line_pool.Append(line_buf.str());
    lines.push_back(line);
    text_buf.Append(line).Append('\n');
  }

  std::cerr << " raw: ";
  TestFileIO("test-file-io.dat", text_buf.str(), lines);
  std::cerr << " gzip: ";
  TestFileIO("test-file-io.dat.gz", text_buf.str(), lines);
  std::cerr << " bzip2: ";
  TestFileIO("test-file-io.dat.bz2", text_buf.str(), lines);
  std::cerr << " xz: ";
  TestFileIO("test-file-io.dat.xz", text_buf.str(), lines);

  return 0;
}
