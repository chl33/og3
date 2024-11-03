// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <inttypes.h>

#include <array>
#include <functional>

namespace og3 {

// Base class for SortedRing and RingQueue
template <typename T, unsigned CAPACITY>
class RingBufferBase {
 public:
  // Capacity is the maximum number number of elements which can be queued.
  RingBufferBase() {}
  RingBufferBase(const RingBufferBase&) = delete;

  // The maximum number number of elements which can be queued.
  std::size_t capacity() const { return m_store.size(); }

  // The number of elements currently queued.
  std::size_t size() const { return m_size; }
  bool empty() const { return size() == 0; }
  bool full() const { return size() == capacity(); }

  T& element(std::size_t idx) { return m_store[index(idx)]; }
  const T& element(std::size_t idx) const { return m_store[index(idx)]; }

  T& operator[](size_t idx) { return m_store[index(idx)]; }
  const T& operator[](size_t idx) const { return m_store[index(idx)]; }

  const T& front() const { return element(0); }
  T& front() { return element(0); }

  const T& back() const { return element(size() - 1); }
  T& back() { return element(size() - 1); }

  std::size_t start_idx() const { return m_start_idx; }

  void popFront();
  void popBack();
  bool remove(const std::function<bool(const T&)>& select_fn);
  bool remove_index(size_t index);

 protected:
  std::size_t index(std::size_t idx) const { return (m_start_idx + idx) % capacity(); }
  void shift(std::size_t num);
  void lshift(std::size_t num);
  void incrementStart() { m_start_idx = (m_start_idx + 1) % capacity(); }
  void decrementStart() { m_start_idx = (m_start_idx + (capacity() - 1)) % capacity(); }

  std::array<T, CAPACITY> m_store;
  std::size_t m_start_idx = 0;
  std::size_t m_size = 0;
};

// A queue with fixed maximum capacity where elements may be added and removed from the ends.
template <typename T, unsigned CAPACITY>
class RingQueue : public RingBufferBase<T, CAPACITY> {
 public:
  // Capacity is the maximum number number of elements which can be queued.
  RingQueue() {}
  RingQueue(const RingQueue&) = delete;

  void pushFront(const T& el);
  void pushBack(const T& el);

  using Parent = RingBufferBase<T, CAPACITY>;
  void popFront() { Parent::popFront(); }
  void popBack() { Parent::popBack(); }
};

// A queue with fixed maximum capacity, where elements are stored in sorted order.
// Elements must be comparable with the '<' operator.
template <typename T, unsigned CAPACITY>
class SortedRing : public RingBufferBase<T, CAPACITY> {
 public:
  // Capacity is the maximum number number of elements which can be queued.
  SortedRing() {}
  SortedRing(const SortedRing&) = delete;

  bool insert(const T& t);
  bool insert(const T& t, size_t index);

  using Parent = RingBufferBase<T, CAPACITY>;
  void popFront() { Parent::popFront(); }
  void popBack() { Parent::popBack(); }
};

template <typename T, unsigned CAPACITY>
inline void RingBufferBase<T, CAPACITY>::shift(std::size_t num) {
  for (std::size_t i = 0; i < num; i++) {
    const std::size_t i_src = size() - i - 1;
    const std::size_t i_dst = i_src + 1;
    if (i_dst < capacity()) {
      element(i_dst) = element(i_src);
    }
  }
}

template <typename T, unsigned CAPACITY>
inline void RingBufferBase<T, CAPACITY>::lshift(std::size_t num) {
  for (std::size_t i = num; i > 0; i--) {
    const std::size_t i_dst = size() - i - 1;
    const std::size_t i_src = i_dst + 1;
    if (i_dst < capacity()) {
      element(i_dst) = element(i_src);
    }
  }
}

template <typename T, unsigned CAPACITY>
inline void RingBufferBase<T, CAPACITY>::popFront() {
  if (empty()) {
    return;
  }
  if (size() == 1) {
    m_start_idx = 0;
    m_size = 0;
    return;
  }
  incrementStart();
  m_size -= 1;
}

template <typename T, unsigned CAPACITY>
inline void RingBufferBase<T, CAPACITY>::popBack() {
  if (empty()) {
    return;
  }
  if (size() == 1) {
    m_start_idx = 0;
    m_size = 0;
    return;
  }
  m_size -= 1;
}

template <typename T, unsigned CAPACITY>
inline bool RingBufferBase<T, CAPACITY>::remove(const std::function<bool(const T&)>& select_fn) {
  std::size_t num_shift = 0;
  while (num_shift < size()) {
    const unsigned idx = size() - num_shift - 1;
    T& el = element(idx);
    if (select_fn(el)) {
      if (idx == 0) {
        incrementStart();
      } else {
        lshift(num_shift);
      }
      m_size -= 1;
      if (m_size == 0) {
        m_start_idx = 0;
      }
      return true;
    }
    num_shift += 1;
  }
  return false;
}

template <typename T, unsigned CAPACITY>
inline bool RingBufferBase<T, CAPACITY>::remove_index(size_t index) {
  if (index >= size()) {
    return false;
  }
  if (index == 0) {
    incrementStart();
  } else {
    lshift(size() - index - 1);
  }
  m_size -= 1;
  return true;
}

template <typename T, unsigned CAPACITY>
inline void RingQueue<T, CAPACITY>::pushFront(const T& el) {
  if (!Parent::empty()) {
    Parent::decrementStart();
  }
  Parent::element(0) = el;
  if (!Parent::full()) {
    Parent::m_size += 1;
  }
}

template <typename T, unsigned CAPACITY>
inline void RingQueue<T, CAPACITY>::pushBack(const T& el) {
  if (Parent::full()) {
    Parent::incrementStart();
    Parent::element(Parent::size() - 1) = el;
  } else {
    Parent::element(Parent::size()) = el;
    Parent::m_size += 1;
  }
}

template <typename T, unsigned CAPACITY>
bool SortedRing<T, CAPACITY>::insert(const T& t) {
  if (Parent::size() == Parent::capacity() || Parent::capacity() < 1) {
    return false;
  }
  if (Parent::size() == 0) {
    // Front element in the queue.
    Parent::m_start_idx = 0;
    Parent::m_size = 1;
    Parent::front() = t;
    return true;
  }
  // If the front element in the list is not before the new element, just insert the
  //  new element at the front of the list.
  if (t < Parent::front()) {
    Parent::decrementStart();
    Parent::front() = t;
    Parent::m_size = std::min(Parent::capacity(), Parent::m_size + 1);
    return true;
  }
  if (Parent::full() && Parent::back() < t) {
    // Don't add the task if full and all current tasks are before it.
    return false;
  }
  std::size_t num_shift = 0;
  while (num_shift < Parent::size()) {
    T& el = Parent::element(Parent::size() - num_shift - 1);
    if (t > el) {
      break;
    }
    num_shift += 1;
  }
  Parent::shift(num_shift);
  T& el = Parent::element(Parent::size() - num_shift);
  el = t;
  Parent::m_size = std::min(Parent::capacity(), Parent::m_size + 1);
  return true;
}

template <typename T, unsigned CAPACITY>
bool SortedRing<T, CAPACITY>::insert(const T& t, size_t index) {
  if (index > Parent::size()) {
    return false;

    if (Parent::capacity() < 1) {
      return false;
    }
    Parent::element(index) = t;
    if (index == Parent::size()) {
      Parent::m_size += 1;
    }
    return true;
  }
}

}  // namespace og3
