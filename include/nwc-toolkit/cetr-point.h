// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CETR_POINT_H_
#define NWC_TOOLKIT_CETR_POINT_H_

namespace nwc_toolkit {

class CetrPoint {
 public:
  CetrPoint() : x_(0.0), y_(0.0) {}
  ~CetrPoint() {
    Clear();
  }

  double x() const {
    return x_;
  }
  double y() const {
    return y_;
  }

  void set_x(double value) {
    x_ = value;
  }
  void set_y(double value) {
    y_ = value;
  }

  void Clear() {
    x_ = 0.0;
    y_ = 0.0;
  }

 private:
  double x_;
  double y_;

  // Copyable.
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CETR_POINT_H_
