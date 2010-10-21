// Copyright 2010 Susumu Yata <syata@acm.org>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include <nwc-toolkit/heap-queue.h>

int main() {
  enum { NUM_VALUES = 1 << 16 };
  enum { MAX_VALUE = 1 << 16 };

  std::srand(static_cast<unsigned int>(std::time(NULL)));

  std::vector<int> values;
  for (std::size_t i = 0; i < NUM_VALUES; ++i) {
    int value = std::rand() % (MAX_VALUE + 1);
    values.push_back(value);
  }

  nwc_toolkit::HeapQueue<int> queue;
  assert(queue.is_empty());
  assert(queue.num_objs() == 0);

  for (std::size_t i = 0; i < values.size(); ++i) {
    queue.Enqueue(values[i]);
    assert(queue.is_empty() == false);
    assert(queue.num_objs() == i + 1);
  }

  std::vector<int> results;
  while (!queue.is_empty()) {
    results.push_back(queue.top());
    queue.Dequeue();
  }
  assert(queue.is_empty());
  assert(queue.num_objs() == 0);

  assert(results.size() == values.size());

  std::sort(values.begin(), values.end());
  for (std::size_t i = 0; i < values.size(); ++i) {
    assert(results[i] == values[i]);
  }

  queue.Enqueue(10);
  assert(queue.top() == 10);
  queue.Enqueue(13);
  assert(queue.top() == 10);
  queue.Enqueue(5);
  assert(queue.top() == 5);
  queue.Enqueue(3);
  assert(queue.top() == 3);
  queue.Replace(2);
  assert(queue.top() == 2);
  queue.Replace(7);
  assert(queue.top() == 5);
  queue.Dequeue();
  assert(queue.top() == 7);

  return 0;
}
