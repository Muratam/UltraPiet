#ifndef INTEGERRANGE_H
#define INTEGERRANGE_H

#include <iterator>
#include <tuple>

template <typename>
class IntegerRange;

namespace iota {
template <typename INT>
class IntegerIterator : public std::iterator<std::input_iterator_tag, INT> {
  template <typename> friend class IntegerRange;
  IntegerIterator(INT current, INT step)
      : current_(current), step_(step) {}
 public:
  const INT operator*() const {
    return current_;
  }
  IntegerIterator& operator++() {
    next();
    return *this;
  }
  IntegerIterator operator++(int) {
    const auto result = *this;
    next();
    return result;
  }
  bool operator==(const IntegerIterator& rhs) const {
    return *(*this) == *rhs;
  }
  bool operator!=(const IntegerIterator& rhs) const {
    return !(*this == rhs);
  }
 private:
  void next() {
    current_ += step_;
  }
  INT current_;
  INT step_;
};

template <typename INT>
class IntegerRange {
 public:
  using iterator = IntegerIterator<INT>;
  IntegerRange(INT b, INT e, INT s)
      : begin_(b), end_(e), step_(s) {
    normalize();
  }
  iterator begin() const {
    return iterator(begin_, step_);
  }
  iterator end() const {
    return iterator(end_, step_);
  }
  void set_end(INT e) {
    end_ = e;
    normalize();
  }
  void set_step(INT s) {
    step_ = s;
    normalize();
  }
 private:
  void normalize() {
    if (begin_ < end_) {
      if (step_ < 0) {
        step_ = -step_;
      }
      end_ += step_ - 1;
    }
    if (end_ < begin_) {
      if (0 < step_) {
        step_ = -step_;
      }
      end_ += step_ + 1;
    }
    end_ = begin_ + ((end_ - begin_) / step_) * step_;
  }
  INT begin_;
  INT end_;
  INT step_;
};

template <typename INT>
IntegerRange<INT> range(INT begin, INT end, INT step) {
  return IntegerRange<INT>(begin, end, step);
}
template <typename INT>
IntegerRange<INT> range(INT begin, INT end) {
  return IntegerRange<INT>(begin, end, 1);
}
template <typename INT>
IntegerRange<INT> range(INT end) {
  return IntegerRange<INT>(0, end, 1);
}

template <typename INT>
class From {
 public:
  using iterator = typename IntegerRange<INT>::iterator;
  From(INT b)
      : range_(b, b, 1) {}
  From& to(INT e) {
    range_.set_end(e);
    return *this;
  }
  From& step(INT s) {
    range_.set_step(s);
    return *this;
  }
  iterator begin() const {
    return range_.begin();
  }
  iterator end() const {
    return range_.end();
  }
 private:
  IntegerRange<INT> range_;
};

template <typename INT>
From<INT> from(INT begin) {
  return From<INT>(begin);
}
}


#endif // INTEGERRANGE_H
