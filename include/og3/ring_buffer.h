// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <inttypes.h>

#include <array>
#include <functional>

namespace og3 {

/**
 * @brief Base class for circular buffer implementations.
 *
 * Provides common indexing and size management for RingQueue and SortedRing.
 * @tparam T The element type.
 * @tparam CAPACITY The maximum number of elements.
 */
template <typename T, unsigned CAPACITY>
class RingBufferBase {
 public:
  RingBufferBase() {}
  RingBufferBase(const RingBufferBase&) = delete;

  /** @return The maximum capacity of the buffer. */
  std::size_t capacity() const { return m_store.size(); }

  /** @return The current number of elements in the buffer. */
  std::size_t size() const { return m_size; }
  /** @return true if the buffer is empty. */
  bool empty() const { return size() == 0; }
  /** @return true if the buffer is at capacity. */
  bool full() const { return size() == capacity(); }

  /** @return Reference to an element by logical index. */
  T& element(std::size_t idx) { return m_store[index(idx)]; }
  /** @return Constant reference to an element by logical index. */
  const T& element(std::size_t idx) const { return m_store[index(idx)]; }

  /** @brief Logical index operator. */
  T& operator[](size_t idx) { return m_store[index(idx)]; }
  /** @brief Logical index operator. */
  const T& operator[](size_t idx) const { return m_store[index(idx)]; }

  /** @return Reference to the first element. */
  const T& front() const { return element(0); }
  /** @return Reference to the first element. */
  T& front() { return element(0); }

  /** @return Reference to the last element. */
  const T& back() const { return element(size() - 1); }
  /** @return Reference to the last element. */
  T& back() { return element(size() - 1); }

  /** @return The current internal starting index. */
  std::size_t start_idx() const { return m_start_idx; }

  /** @brief Removes the first element. */
  void popFront();
  /** @brief Removes the last element. */
  void popBack();

  /**
   * @brief Removes the first element that matches a predicate.
   * @param select_fn Predicate function.
   * @return true if an element was removed.
   */
  bool remove(const std::function<bool(const T&)>& select_fn);

  /**
   * @brief Removes an element at a specific logical index.
   * @param index Logical index.
   * @return true if successful.
   */
  bool remove_index(size_t index);

 protected:
  /** @brief Converts logical index to internal array index. */
  std::size_t index(std::size_t idx) const { return (m_start_idx + idx) % capacity(); }
  void shift(std::size_t num);
  void lshift(std::size_t num);
  void incrementStart() { m_start_idx = (m_start_idx + 1) % capacity(); }
  void decrementStart() { m_start_idx = (m_start_idx + (capacity() - 1)) % capacity(); }

  std::array<T, CAPACITY> m_store;
  std::size_t m_start_idx = 0;
  std::size_t m_size = 0;
};

/**
 * @brief A fixed-capacity Double-Ended Queue (Deque) implemented as a ring buffer.
 */
template <typename T, unsigned CAPACITY>
class RingQueue : public RingBufferBase<T, CAPACITY> {
 public:
  RingQueue() {}
  RingQueue(const RingQueue&) = delete;

  /** @brief Adds an element to the front. */
  void pushFront(const T& el);
  /** @brief Adds an element to the back. */
  void pushBack(const T& el);

  using Parent = RingBufferBase<T, CAPACITY>;
  /** @brief Removes the first element. */
  void popFront() { Parent::popFront(); }
  /** @brief Removes the last element. */
  void popBack() { Parent::popBack(); }
};

/**
 * @brief A fixed-capacity buffer that maintains elements in sorted order.
 *
 * Elements must implement `operator<` and `operator>`.
 */
template <typename T, unsigned CAPACITY>
class SortedRing : public RingBufferBase<T, CAPACITY> {
 public:
  SortedRing() {}
  SortedRing(const SortedRing&) = delete;

  /**
   * @brief Inserts an element while maintaining sort order.
   * @param t The element to insert.
   * @return true if inserted, false if buffer is full and element is out of range.
   */
  bool insert(const T& t);

  /**
   * @brief Inserts at a specific logical index (expert use).
   * @param t The element.
   * @param index Logical index.
   * @return true if successful.
   */
  bool insert(const T& t, size_t index);

  using Parent = RingBufferBase<T, CAPACITY>;
  /** @brief Removes the front element. */
  void popFront() { Parent::popFront(); }
  /** @brief Removes the back element. */
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
