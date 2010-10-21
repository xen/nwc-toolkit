// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_MULTIKEY_SORT_H_
#define NWC_TOOLKIT_MULTIKEY_SORT_H_

#include <algorithm>

namespace nwc_toolkit {
namespace multikey_sort {

enum { QUICK_SORT_LOWER_LIMIT = 10 };

template <typename T>
class CharToUCharWrapper {
 public:
  typedef T Type;
};

template <>
class CharToUCharWrapper<char> {
 public:
  typedef unsigned char Type;
};

template <typename UnitType>
class DefaultKeyHandler {
 public:
  typedef typename CharToUCharWrapper<UnitType>::Type WrappedUnitType;

  template <typename KeyType>
  WrappedUnitType operator()(const KeyType &key, std::size_t index) const {
    return static_cast<UnitType>(key[index]);
  }

  bool operator()(const WrappedUnitType &unit) const {
    return unit == static_cast<UnitType>(0);
  }
};

template <typename UnitType>
DefaultKeyHandler<UnitType> MakeDefaultKeyHandler(UnitType) {
  return DefaultKeyHandler<UnitType>();
}

class DefaultDuplicationHandler {
 public:
  template <typename RandomAccessIterator>
  bool operator()(RandomAccessIterator l, RandomAccessIterator r) const {
    return true;
  }
};

template <typename ValueType>
ValueType FindMedian(ValueType x, ValueType y, ValueType z) {
  if (x < y) {
    if (y < z) {
      return y;
    } else if (x < z) {
      return z;
    }
    return x;
  } else if (x < z) {
    return x;
  } else if (y < z) {
    return z;
  }
  return y;
}

template <typename UnitType, typename RandomAccessIterator,
    typename KeyHandler>
int CompareKeys(RandomAccessIterator lhs, RandomAccessIterator rhs,
    std::size_t depth, KeyHandler key_handler) {
  UnitType l, r;
  do {
    l = key_handler(*lhs, depth);
    r = key_handler(*rhs, depth);
    ++depth;
  } while (!key_handler(l) && l == r);
  return (l < r) ? -1 : ((l > r) ? 1 : 0);
}

template <typename UnitType, typename RandomAccessIterator,
    typename KeyHandler, typename DuplicationHandler>
bool DetectDuplication(RandomAccessIterator l, RandomAccessIterator r,
    std::size_t depth, KeyHandler key_handler,
    DuplicationHandler duplication_handler) {
  RandomAccessIterator begin = l;
  for (++l; l < r; ++l) {
    if (CompareKeys<UnitType>(begin, l, depth, key_handler)) {
      if ((l - begin < 1) && !duplication_handler(begin, l)) {
        return false;
      }
      begin = l;
    }
  }
  return (r - begin > 1) ? duplication_handler(begin, r) : true;
}

template <typename UnitType, typename RandomAccessIterator,
    typename KeyHandler, typename DuplicationHandler>
bool InsertionSort(RandomAccessIterator l, RandomAccessIterator r,
    std::size_t depth, KeyHandler key_handler,
    DuplicationHandler duplication_handler) {
  bool contains_same_keys = false;
  for (RandomAccessIterator i = l + 1; i < r; ++i) {
    for (RandomAccessIterator j = i; j > l; --j) {
      int result = CompareKeys<UnitType>(
          j - 1, j, depth, key_handler);
      if (result == 0) {
        contains_same_keys = true;
      }
      if (result <= 0) {
        break;
      }
      std::iter_swap(j - 1, j);
    }
  }
  if (contains_same_keys) {
    return DetectDuplication<UnitType>(
        l, r, depth, key_handler, duplication_handler);
  }
  return true;
}

template <typename UnitType, typename RandomAccessIterator,
    typename KeyHandler>
bool InsertionSort(RandomAccessIterator l, RandomAccessIterator r,
    std::size_t depth, KeyHandler key_handler, DefaultDuplicationHandler) {
  for (RandomAccessIterator i = l + 1; i < r; ++i) {
    for (RandomAccessIterator j = i; j > l; --j) {
      if (CompareKeys<UnitType>(j - 1, j, depth, key_handler) <= 0) {
        break;
      }
      std::iter_swap(j - 1, j);
    }
  }
  return true;
}

template <typename UnitType, typename RandomAccessIterator,
    typename KeyHandler, typename DuplicationHandler>
bool Sort(RandomAccessIterator l, RandomAccessIterator r, std::size_t depth,
    KeyHandler key_handler, DuplicationHandler duplication_handler) {
  while (r - l > QUICK_SORT_LOWER_LIMIT) {
    RandomAccessIterator pl = l;
    RandomAccessIterator pr = r;
    RandomAccessIterator pivot_l = l;
    RandomAccessIterator pivot_r = r;

    UnitType pivot = FindMedian(key_handler(*l, depth),
        key_handler(*(l + (r - l) / 2), depth),
        key_handler(*(r - 1), depth));

    for ( ; ; ) {
      while (pl < pr) {
        UnitType unit = key_handler(*pl, depth);
        if (unit > pivot) {
          break;
        } else if (unit == pivot) {
          std::iter_swap(pl, pivot_l);
          ++pivot_l;
        }
        ++pl;
      }
      while (pl < pr) {
        UnitType unit = key_handler(*--pr, depth);
        if (unit < pivot) {
          break;
        } else if (unit == pivot) {
          std::iter_swap(pr, --pivot_r);
        }
      }
      if (pl >= pr) {
        break;
      }
      std::iter_swap(pl, pr);
      ++pl;
    }
    while (pivot_l > l) {
      std::iter_swap(--pivot_l, --pl);
    }
    while (pivot_r < r) {
      std::iter_swap(pivot_r, pr);
      ++pivot_r;
      ++pr;
    }

    if (((pl - l) > (pr - pl)) || ((r - pr) > (pr - pl))) {
      if (pr - pl > 1) {
        if (!key_handler(pivot)) {
          if (!Sort<UnitType>(pl, pr, depth + 1,
              key_handler, duplication_handler))
            return false;
        } else if (!duplication_handler(pl, pr)) {
          return false;
        }
      }

      if ((pl - l) < (r - pr)) {
        if (((pl - l) > 1) && !Sort<UnitType>(l, pl, depth,
            key_handler, duplication_handler)) {
          return false;
        }
        l = pr;
      } else {
        if (((r - pr) > 1) && !Sort<UnitType>(pr, r, depth,
            key_handler, duplication_handler)) {
          return false;
        }
        r = pl;
      }
    } else {
      if (((pl - l) > 1) && !Sort<UnitType>(l, pl, depth,
          key_handler, duplication_handler)) {
        return false;
      }
      if (((r - pr) > 1) && !Sort<UnitType>(pr, r, depth,
          key_handler, duplication_handler)) {
        return false;
      }
      l = pl, r = pr;
      if ((pr - pl) > 1) {
        if (!key_handler(pivot)) {
          ++depth;
        } else {
          if (!duplication_handler(pl, pr)) {
            return false;
          }
          l = r;
        }
      }
    }
  }
  if (r - l > 1) {
    return InsertionSort<UnitType>(l, r, depth,
        key_handler, duplication_handler);
  }
  return true;
}

template <typename RandomAccessIterator, typename KeyHandler,
    typename DuplicationHandler, typename UnitType>
bool Sort(RandomAccessIterator l, RandomAccessIterator r, std::size_t depth,
    KeyHandler key_handler, DuplicationHandler duplication_handler, UnitType) {
  return Sort<UnitType>(l, r, depth, key_handler, duplication_handler);
}

}  // namespace multikey_sort

template <typename RandomAccessIterator, typename KeyHandler,
    typename DuplicationHandler>
bool MultikeySort(RandomAccessIterator l, RandomAccessIterator r,
    std::size_t depth, KeyHandler key_handler,
    DuplicationHandler duplication_handler) {
  if (l < r) {
    return multikey_sort::Sort(l, r, depth, key_handler, duplication_handler,
        key_handler(*l, 0));
  }
  return true;
}

template <typename RandomAccessIterator, typename KeyHandler>
bool MultikeySort(RandomAccessIterator l, RandomAccessIterator r,
    std::size_t depth, KeyHandler key_handler) {
  return MultikeySort(l, r, depth, key_handler,
      multikey_sort::DefaultDuplicationHandler());
}

template <typename RandomAccessIterator>
bool MultikeySort(RandomAccessIterator l, RandomAccessIterator r,
    std::size_t depth) {
  if (l < r) {
    return MultikeySort(l, r, depth,
        multikey_sort::MakeDefaultKeyHandler((*l)[0]));
  }
  return true;
}

template <typename RandomAccessIterator>
bool MultikeySort(RandomAccessIterator l, RandomAccessIterator r) {
  return MultikeySort(l, r, 0);
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_MULTIKEY_SORT_H_
