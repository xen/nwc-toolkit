// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CETR_DOCUMENT_H_
#define NWC_TOOLKIT_CETR_DOCUMENT_H_

#include <vector>

#include "./cetr-line.h"
#include "./cetr-point.h"
#include "./cetr-unit.h"
#include "./html-document.h"

namespace nwc_toolkit {

class CetrDocument {
 public:
  enum {
    MIN_NUM_CHARS_THRESHOLD = 1,
    DEFAULT_NUM_CHARS_THRESHOLD = 65
  };

  enum {
    MIN_WINDOW_SIZE = 1,
    DEFAULT_WINDOW_SIZE = 3
  };

  CetrDocument();
  ~CetrDocument() {
    Clear();
  }

  std::size_t num_lines() const {
    return lines_.size();
  }
  const CetrLine &line(std::size_t id) const {
    return lines_[id];
  }
  std::size_t num_units() const {
    return units_.size();
  }
  const CetrUnit &unit(std::size_t id) const {
    return units_[id];
  }

  double tag_ratio(std::size_t id) const {
    return tag_ratios_[id];
  }
  double smoothed_tag_ratio(std::size_t id) const {
    return smoothed_tag_ratios_[id];
  }
  double derivate(std::size_t id) const {
    return derivates_[id];
  }
  double smoothed_derivate(std::size_t id) const {
    return smoothed_derivates_[id];
  }

  std::size_t num_points() const {
    return points_.size();
  }
  const CetrPoint &point(std::size_t id) const {
    return points_[id];
  }
  int cluster(std::size_t id) const {
    return clusters_[id];
  }
  bool is_content(std::size_t id) const {
    return clusters_[id] != 0;
  }

  void Clear();

  void Parse(const HtmlDocument &doc,
      std::size_t num_chars_threshold = 0,
      int window_size = 0, int num_clusters = 0,
      int num_tries = 0, int num_steps = 0);

 private:
  std::vector<CetrLine> lines_;
  std::vector<CetrUnit> units_;
  std::vector<double> tag_ratios_;
  std::vector<double> smoothed_tag_ratios_;
  std::vector<double> derivates_;
  std::vector<double> smoothed_derivates_;
  std::vector<CetrPoint> points_;
  std::vector<int> clusters_;

  void ExtractLines(const HtmlDocument &doc,
      std::size_t num_chars_threshold);
  void ComputeTagRatios();
  void SmoothTagRatios();
  void ComputeDerivates(int window_size);
  void SmoothDerivates();
  void AssemblePoints();
  void ClusterPoints(int num_clusters, int num_tries, int num_steps);

  void AppendTagUnit(const HtmlUnit &html_unit,
      CetrLine *cetr_line);
  void AppendTextUnit(const HtmlUnit &html_unit,
      std::size_t num_chars_threshold, CetrLine *cetr_line);
  void AppendInvisibleUnit(const HtmlUnit &html_unit,
      CetrLine *cetr_line);
  void FixLines();

  static std::size_t CountChars(const String &str);
  static bool ExtractCodePoint(const String &str,
      String *str_left, int *code_point);
  static bool IsPrintable(int code_point);

  static double ComputeStandardDerivation(
      const std::vector<double> &values);
  static void SmoothHistogram(const std::vector<double> &src,
      std::vector<double> *dest);

  // Disallows copy and assignment.
  CetrDocument(const CetrDocument &);
  CetrDocument &operator=(const CetrDocument &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CETR_DOCUMENT_H_
