// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_HEAP_QUEUE_H_
#define NWC_TOOLKIT_HEAP_QUEUE_H_

#include <functional>
#include <utility>
#include <vector>

namespace nwc_toolkit {

template <typename T, typename LessThan = std::less<T> >
class HeapQueue {
 public:
  HeapQueue() : buf_() {}
  ~HeapQueue() {}

  const T &top() const {
    return buf_[root_index()];
  }

  bool is_empty() const {
    return buf_.empty();
  }
  std::size_t num_objs() const {
    return buf_.size();
  }

  void Clear() {
    buf_.clear();
  }

  void Enqueue(const T &value);
  void Dequeue();

  void Replace(const T &value);

 private:
  std::vector<T> buf_;

  static std::size_t root_index() {
    return 0;
  }
  static std::size_t GetParentIndex(std::size_t index) {
    return (index - 1) / 2;
  }
  static std::size_t GetLeftChildIndex(std::size_t index) {
    return (index * 2) + 1;
  }

  // Disallows copy and assignment.
  HeapQueue(const HeapQueue &);
  HeapQueue &operator=(const HeapQueue &);
};

template <typename T, typename LessThan>
void HeapQueue<T, LessThan>::Enqueue(const T &value) {
  std::size_t index = buf_.size();
  buf_.resize(buf_.size() + 1);
  while (index > root_index()) {
    std::size_t parent_index = GetParentIndex(index);
    if (!LessThan()(value, buf_[parent_index])) {
      break;
    }
    buf_[index] = buf_[parent_index];
    index = parent_index;
  }
  buf_[index] = value;
}

template <typename T, typename LessThan>
void HeapQueue<T, LessThan>::Dequeue() {
  const T &value = buf_.back();
  Replace(value);
  buf_.pop_back();
}

template <typename T, typename LessThan>
void HeapQueue<T, LessThan>::Replace(const T &value) {
  std::size_t index = root_index();
  for ( ; ; ) {
    std::size_t child_index = GetLeftChildIndex(index);
    if (child_index >= buf_.size()) {
      break;
    }
    if ((child_index + 1 < buf_.size()) &&
      LessThan()(buf_[child_index + 1], buf_[child_index])) {
      ++child_index;
    }
    if (!LessThan()(buf_[child_index], value)) {
      break;
    }
    buf_[index] = buf_[child_index];
    index = child_index;
  }
  buf_[index] = value;
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_HEAP_QUEUE_H_
