// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/cetr-document.h>

#include <cmath>

#include <nwc-toolkit/cetr-cluster.h>

namespace nwc_toolkit {

CetrDocument::CetrDocument()
    : lines_(),
      units_(),
      tag_ratios_(),
      smoothed_tag_ratios_(),
      derivates_(),
      smoothed_derivates_(),
      points_(),
      clusters_() {}

void CetrDocument::Clear() {
  lines_.clear();
  units_.clear();
  tag_ratios_.clear();
  smoothed_tag_ratios_.clear();
  derivates_.clear();
  smoothed_derivates_.clear();
  points_.clear();
  clusters_.clear();
}

void CetrDocument::Parse(const HtmlDocument &doc,
    std::size_t num_chars_threshold, int window_size,
    int num_clusters, int num_tries, int num_steps) {
  Clear();

  ExtractLines(doc, num_chars_threshold);
  ComputeTagRatios();
  SmoothTagRatios();
  ComputeDerivates(window_size);
  SmoothDerivates();
  AssemblePoints();
  ClusterPoints(num_clusters, num_tries, num_steps);
}

void CetrDocument::ExtractLines(const HtmlDocument &doc,
    std::size_t num_chars_threshold) {
  if (num_chars_threshold < MIN_NUM_CHARS_THRESHOLD) {
    num_chars_threshold = DEFAULT_NUM_CHARS_THRESHOLD;
  } else if (num_chars_threshold > MAX_NUM_CHARS_THRESHOLD) {
    num_chars_threshold = MAX_NUM_CHARS_THRESHOLD;
  }
  for (std::size_t i = 0; i < doc.num_units(); ++i) {
    if (doc.unit(i).src().Contains("\r\n")) {
      num_chars_threshold = MAX_NUM_CHARS_THRESHOLD;
      break;
    }
  }

  CetrLine line;
  bool is_visible = true;
  for (std::size_t i = 0; i < doc.num_units(); ++i) {
    const HtmlUnit &html_unit = doc.unit(i);
    switch (html_unit.type()) {
      case HtmlUnit::TEXT_UNIT: {
        if (is_visible) {
          AppendTextUnit(html_unit, num_chars_threshold, &line);
        } else {
          AppendInvisibleUnit(html_unit, &line);
        }
        break;
      }
      case HtmlUnit::TAG_UNIT: {
        if ((html_unit.tag_name() == "script") ||
            (html_unit.tag_name() == "style")) {
          is_visible = (html_unit.is_start_tag() == false);
          AppendInvisibleUnit(html_unit, &line);
        } else if (line.num_chars() >= num_chars_threshold) {
          lines_.push_back(line);
          line.Clear();
        } else {
          AppendTagUnit(html_unit, &line);
        }
        break;
      }
      case HtmlUnit::COMMENT_UNIT:
      case HtmlUnit::OTHER_UNIT: {
        AppendInvisibleUnit(html_unit, &line);
        break;
      }
      default: {
        break;
      }
    }
  }
  if (line.num_units() != 0) {
    lines_.push_back(line);
  }
  FixLines();
}

void CetrDocument::ComputeTagRatios() {
  tag_ratios_.resize(lines_.size());
  for (std::size_t i = 0; i < lines_.size(); ++i) {
    tag_ratios_[i] = 1.0 * lines_[i].num_text_chars()
        / ((lines_[i].num_tags() != 0) ? lines_[i].num_tags() : 1);
  }
}

void CetrDocument::SmoothTagRatios() {
  SmoothHistogram(tag_ratios_, &smoothed_tag_ratios_);
}

void CetrDocument::ComputeDerivates(int window_size) {
  if (window_size < MIN_WINDOW_SIZE) {
    window_size = DEFAULT_WINDOW_SIZE;
  }

  derivates_.resize(lines_.size(), 0.0);
  for (std::size_t i = 0; i < derivates_.size(); ++i) {
    int current_window_size = ((i + window_size) < derivates_.size())
        ? window_size : (derivates_.size() - i);

    for (int j = 0; j < current_window_size; ++j) {
      derivates_[i] += smoothed_tag_ratios_[i + j];
    }
    derivates_[i] /= current_window_size;
    derivates_[i] -= smoothed_tag_ratios_[i];
  }
}

void CetrDocument::SmoothDerivates() {
  SmoothHistogram(derivates_, &smoothed_derivates_);
}

void CetrDocument::AssemblePoints() {
  points_.resize(lines_.size());
  for (std::size_t i = 0; i < points_.size(); ++i) {
    points_[i].set_x(std::fabs(smoothed_derivates_[i]));
    points_[i].set_y(smoothed_tag_ratios_[i]);
  }
}

void CetrDocument::ClusterPoints(int num_clusters,
    int num_tries, int num_steps) {
  CetrCluster::Cluster(points_, &clusters_, num_clusters,
      num_tries, num_steps);
}

void CetrDocument::AppendTagUnit(const HtmlUnit &html_unit,
    CetrLine *cetr_line) {
  CetrUnit cetr_unit;
  cetr_unit.set_type(CetrUnit::TAG_UNIT);
  cetr_unit.set_unit(html_unit);
  cetr_unit.set_content(html_unit.src());
  units_.push_back(cetr_unit);

  std::size_t num_chars = CountChars(html_unit.src());
  cetr_line->set_num_units(cetr_line->num_units() + 1);
  cetr_line->set_num_tags(cetr_line->num_tags() + 1);
  cetr_line->set_num_chars(cetr_line->num_chars() + num_chars);
}

void CetrDocument::AppendTextUnit(const HtmlUnit &html_unit,
    std::size_t num_chars_threshold, CetrLine *cetr_line) {
  CetrUnit cetr_unit;
  cetr_unit.set_type(CetrUnit::TEXT_UNIT);
  cetr_unit.set_unit(html_unit);

  String str_left = html_unit.text_content();
  String text = str_left.SubString(0, 0);
  int code_point = 0;
  while (ExtractCodePoint(str_left, &str_left, &code_point)) {
    if (IsPrintable(code_point)) {
      if (cetr_line->num_chars() >= num_chars_threshold) {
        if (!text.is_empty()) {
          cetr_unit.set_content(text);
          units_.push_back(cetr_unit);
          cetr_line->set_num_units(cetr_line->num_units() + 1);
          text.set_begin(text.end());
        }
        lines_.push_back(*cetr_line);
        cetr_line->Clear();
      }
      cetr_line->set_num_chars(cetr_line->num_chars() + 1);
      cetr_line->set_num_text_chars(cetr_line->num_text_chars() + 1);
    }

    text.set_end(str_left.begin());
    if ((code_point == '\r') || (code_point == '\n')) {
      cetr_unit.set_content(text);
      units_.push_back(cetr_unit);
      cetr_line->set_num_units(cetr_line->num_units() + 1);
      text = str_left.SubString(0, 0);
      lines_.push_back(*cetr_line);
      cetr_line->Clear();
    }
  }

  if (!text.is_empty()) {
    cetr_unit.set_content(text);
    units_.push_back(cetr_unit);
    cetr_line->set_num_units(cetr_line->num_units() + 1);
  }
}

void CetrDocument::AppendInvisibleUnit(const HtmlUnit &html_unit,
    CetrLine *cetr_line) {
  CetrUnit cetr_unit;
  cetr_unit.set_type(CetrUnit::INVISIBLE_UNIT);
  cetr_unit.set_unit(html_unit);
  cetr_unit.set_content(html_unit.src());
  units_.push_back(cetr_unit);

  cetr_line->set_num_units(cetr_line->num_units() + 1);
}

std::size_t CetrDocument::CountChars(const String &str) {
  std::size_t num_chars = 0;
  String str_left = str;
  int code_point = 0;
  while (ExtractCodePoint(str_left, &str_left, &code_point)) {
    if (IsPrintable(code_point)) {
      ++num_chars;
    }
  }
  return num_chars;
}

bool CetrDocument::ExtractCodePoint(const String &str,
    String *str_left, int *code_point) {
  if (str.is_empty()) {
    return false;
  }
  *code_point = static_cast<unsigned char>(str[0]);
  std::size_t length = 1;
  if (*code_point >= 0x80) {
    while (*code_point & 0x40) {
      ++length;
      *code_point <<= 1;
    }
    *code_point = (*code_point & 0x3F) >> (length - 1);
    for (std::size_t i = 1; i < length; ++i) {
      *code_point <<= 6;
      *code_point |= static_cast<unsigned char>(str[i]) & 0x3F;
    }
  }
  *str_left = str.SubString(length);
  return true;
}

bool CetrDocument::IsPrintable(int code_point) {
  if (code_point < 0x20) {
    return false;
  }

  switch (code_point) {
    case 0x0020:  // SPACE (U+0020)

    case 0x00A0:  // NO-BREAK SPACE (U+00A0)

    case 0x034F:  // COMBINING GRAPHEME JOINER (U+034F)

    case 0x2000:  // EN QUAD (U+2000)
    case 0x2001:  // EM QUAD (U+2001)
    case 0x2002:  // EN SPACE (U+2002)
    case 0x2003:  // EM SPACE (U+2003)
    case 0x2004:  // THREE-PER-EM SPACE (U+2004)
    case 0x2005:  // FOUR-PER-EM SPACE (U+2005)
    case 0x2006:  // SIX-PER-EM SPACE (U+2006)
    case 0x2007:  // FIGURE SPACE (U+2007)
    case 0x2008:  // PUNCTUATION SPACE (U+2008)
    case 0x2009:  // THIN SPACE (U+2009)
    case 0x200A:  // HAIR SPACE (U+200A)
    case 0x200B:  // ZERO-WIDTH SPACE
    case 0x200C:  // ZERO-WIDTH NON-JOINER
    case 0x200D:  // ZERO-WIDTH JOINER
    case 0x200E:  // LEFT-TO-RIGHT MARK (U+200E)
    case 0x200F:  // RIGHT-TO-LEFT MARK (U+200F)

    case 0x202A:  // LEFT-TO-RIGHT EMBEDDING (U+202A)
    case 0x202B:  // RIGHT-TO-LEFT EMBEDDING (U+202B)
    case 0x202C:  // POP DIRECTIONAL FORMATTING (U+202C)
    case 0x202D:  // LEFT-TO-RIGHT OVERRIDE (U+202D)
    case 0x202E:  // RIGHT-TO-LEFT OVERRIDE (U+202E)
    case 0x202F:  // NARROW NO-BREAK SPACE (U+202F)

    case 0x205F:  // MATHEMATICAL SPACE (U+205F)
    case 0x2060:  // WORD JOINER (U+2060)
    case 0x2061:  // FUNCTION APPLICATION (U+2061)
    case 0x2062:  // INVISIBLE TIMES (U+2062)
    case 0x2063:  // INVISIBLE SEPARATOR (U+2063)

    case 0xFEFF:  // ZERO WIDTH NO-BREAK SPACE (U+FEFF) Byte order mark

    case 0xFFFD:  // REPLACEMENT CHARACTER (U+FFFD)
    case 0xFFFE: {  // Invalid code point
      return false;
    }
    default: {
      return true;
    }
  }
}

void CetrDocument::FixLines() {
  std::size_t unit_id = 0;
  for (std::size_t i = 0; i < lines_.size(); ++i) {
    lines_[i].set_units(&units_[unit_id]);
    unit_id += lines_[i].num_units();
  }
}

void CetrDocument::SmoothHistogram(const std::vector<double> &src,
    std::vector<double> *dest) {
  double standard_derivation = ComputeStandardDerivation(src);
  int radius = static_cast<int>(std::ceil(standard_derivation));

  std::vector<double> kernel_buf((radius * 2) + 1, 0.0);
  double *kernel = &kernel_buf[radius];
  kernel[0] = 1.0;
  if (standard_derivation != 0.0) {
    double denominator = 2.0 * standard_derivation * standard_derivation;
    for (int kernel_id = 1; kernel_id <= radius; ++kernel_id) {
      kernel[kernel_id] = std::exp(-((kernel_id * kernel_id) / denominator));
      kernel[-kernel_id] = kernel[kernel_id];
    }
  }

  double denominator = 0.0;
  for (int kernel_id = 0; kernel_id <= radius; ++kernel_id) {
    denominator += kernel[kernel_id];
  }
  for (int kernel_id = -radius; kernel_id <= radius; ++kernel_id) {
    kernel[kernel_id] /= denominator;
  }

  dest->resize(src.size(), 0.0);
  for (std::size_t dest_id = 0; dest_id < dest->size(); ++dest_id) {
    for (int kernel_id = -radius; kernel_id <= radius; ++kernel_id) {
      int src_id = dest_id + kernel_id;
      if ((src_id >= 0) && (src_id < static_cast<int>(src.size()))) {
        (*dest)[dest_id] += kernel[kernel_id] * src[src_id];
      }
    }
  }
}

double CetrDocument::ComputeStandardDerivation(
    const std::vector<double> &values) {
  if (values.empty()) {
    return 0.0;
  }
  double total = 0.0;
  for (std::size_t i = 0; i < values.size(); ++i) {
    total += values[i];
  }
  double average = total / values.size();
  double variance = 0.0;
  for (std::size_t i = 0; i < values.size(); ++i) {
    double diff = values[i] - average;
    variance += diff * diff;
  }
  variance /= values.size();
  return std::sqrt(variance);
}

}  // namespace nwc_toolkit
