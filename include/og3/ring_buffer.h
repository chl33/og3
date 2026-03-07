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
  /** @brief Removes the front element. */
  void popFront() { Parent::popFront(); }
  /** @brief Removes the back element. */
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

// ... implementation ...
