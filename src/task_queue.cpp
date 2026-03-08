// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/task_queue.h"

#include <algorithm>
#include <cstring>

namespace og3 {

TaskQueue::TaskQueue(std::size_t capacity) : m_pool(capacity) {
  // Initialize the free list
  if (capacity > 0) {
    m_free_head = 0;
    for (std::size_t i = 0; i < capacity - 1; ++i) {
      m_pool[i].next = static_cast<NodeIdx>(i + 1);
    }
    m_pool[capacity - 1].next = kInvalidIdx;
  }
}

bool TaskQueue::insert(unsigned long msec, const Thunk& t, unsigned id) {
  if (full()) {
    return false;
  }
  insertReplace(msec, t, id);
  return true;
}

void TaskQueue::insertReplace(unsigned long msec, const Thunk& t, unsigned id) {
  if (capacity() < 1) {
    return;
  }

  // 1. Remove existing task with same ID
  const bool removed = remove(id);

  // If full and no existing same-ID task was removed, check if we should even add this.
  if (full() && !removed) {
    if (isBefore(m_pool[m_tail].data.msec, msec)) {
      // New task is scheduled after the last one and queue is full.
      return;
    }
    // New task is scheduled before the current tail, so we'll need to drop the tail.
    popLast();
  }

  // 2. Allocate a new node
  NodeIdx newIdx = allocNode();
  if (newIdx == kInvalidIdx) {
    return;
  }
  m_pool[newIdx].data.msec = msec;
  m_pool[newIdx].data.thunk = t;
  m_pool[newIdx].data.id = id;

  // 3. Find insertion point (keep list sorted by time)
  if (m_size == 0) {
    m_head = m_tail = newIdx;
    m_pool[newIdx].prev = m_pool[newIdx].next = kInvalidIdx;
  } else if (isBefore(msec, m_pool[m_head].data.msec)) {
    // Insert at front
    linkBefore(newIdx, m_head);
  } else if (!isBefore(msec, m_pool[m_tail].data.msec)) {
    // Insert at back
    linkAfter(newIdx, m_tail);
  } else {
    // Insert in the middle
    NodeIdx curr = m_head;
    while (curr != kInvalidIdx && isBefore(m_pool[curr].data.msec, msec)) {
      curr = m_pool[curr].next;
    }
    if (curr != kInvalidIdx) {
      linkBefore(newIdx, curr);
    } else {
      linkAfter(newIdx, m_tail);
    }
  }
  m_size++;
}

bool TaskQueue::remove(unsigned id) {
  if (id == 0 || empty()) {
    return false;
  }

  NodeIdx curr = m_head;
  while (curr != kInvalidIdx) {
    if (m_pool[curr].data.id == id) {
      unlink(curr);
      freeNode(curr);
      m_size--;
      return true;
    }
    curr = m_pool[curr].next;
  }
  return false;
}

bool TaskQueue::runNext() {
  if (empty()) {
    return false;
  }
  Thunk fn = m_pool[m_head].data.thunk;
  popFirst();
  if (fn) {
    fn();
  }
  return true;
}

void TaskQueue::popFirst() {
  if (empty()) {
    return;
  }
  NodeIdx oldHead = m_head;
  unlink(oldHead);
  freeNode(oldHead);
  m_size--;
}

void TaskQueue::popLast() {
  if (empty()) {
    return;
  }
  NodeIdx oldTail = m_tail;
  unlink(oldTail);
  freeNode(oldTail);
  m_size--;
}

TimedThunk& TaskQueue::thunk(std::size_t idx) {
  static TimedThunk emptyThunk;
  if (idx >= m_size) {
    return emptyThunk;
  }
  NodeIdx curr = m_head;
  for (std::size_t i = 0; i < idx && curr != kInvalidIdx; ++i) {
    curr = m_pool[curr].next;
  }
  if (curr == kInvalidIdx) return emptyThunk;
  return m_pool[curr].data;
}

const TimedThunk& TaskQueue::thunk(std::size_t idx) const {
  static TimedThunk emptyThunk;
  if (idx >= m_size) {
    return emptyThunk;
  }
  NodeIdx curr = m_head;
  for (std::size_t i = 0; i < idx && curr != kInvalidIdx; ++i) {
    curr = m_pool[curr].next;
  }
  if (curr == kInvalidIdx) return emptyThunk;
  return m_pool[curr].data;
}

void TaskQueue::show() const {
  printf("TaskQueue[%zu/%zu]: ", m_size, capacity());
  NodeIdx curr = m_head;
  while (curr != kInvalidIdx) {
    printf("%lu(%u) ", m_pool[curr].data.msec, m_pool[curr].data.id);
    curr = m_pool[curr].next;
  }
  printf("\n");
}

// -- Private Helpers --

TaskQueue::NodeIdx TaskQueue::allocNode() {
  if (m_free_head == kInvalidIdx) {
    return kInvalidIdx;
  }
  NodeIdx idx = m_free_head;
  m_free_head = m_pool[idx].next;
  m_pool[idx].next = m_pool[idx].prev = kInvalidIdx;
  return idx;
}

void TaskQueue::freeNode(NodeIdx idx) {
  m_pool[idx].next = m_free_head;
  m_pool[idx].prev = kInvalidIdx;
  m_free_head = idx;
}

void TaskQueue::linkBefore(NodeIdx nodeIdx, NodeIdx beforeIdx) {
  NodeIdx prevIdx = m_pool[beforeIdx].prev;
  m_pool[nodeIdx].next = beforeIdx;
  m_pool[nodeIdx].prev = prevIdx;
  m_pool[beforeIdx].prev = nodeIdx;
  if (prevIdx != kInvalidIdx) {
    m_pool[prevIdx].next = nodeIdx;
  } else {
    m_head = nodeIdx;
  }
}

void TaskQueue::linkAfter(NodeIdx nodeIdx, NodeIdx afterIdx) {
  NodeIdx nextIdx = m_pool[afterIdx].next;
  m_pool[nodeIdx].prev = afterIdx;
  m_pool[nodeIdx].next = nextIdx;
  m_pool[afterIdx].next = nodeIdx;
  if (nextIdx != kInvalidIdx) {
    m_pool[nextIdx].prev = nodeIdx;
  } else {
    m_tail = nodeIdx;
  }
}

void TaskQueue::unlink(NodeIdx idx) {
  NodeIdx prevIdx = m_pool[idx].prev;
  NodeIdx nextIdx = m_pool[idx].next;

  if (prevIdx != kInvalidIdx) {
    m_pool[prevIdx].next = nextIdx;
  } else {
    m_head = nextIdx;
  }

  if (nextIdx != kInvalidIdx) {
    m_pool[nextIdx].prev = prevIdx;
  } else {
    m_tail = prevIdx;
  }
}

}  // namespace og3
