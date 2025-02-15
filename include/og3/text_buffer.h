// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <stdarg.h>
#include <stdio.h>

#include <array>

namespace og3 {

template <int LEN>
class TextBuffer {
 public:
  TextBuffer() { clear(); }
  void clear() {
    m_len = 0;
    m_buffer[0] = 0;
  }
  unsigned size() const { return sizeof(m_buffer); }
  const char* text() const { return m_buffer; }
  int add(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int ret = addv(fmt, ap);
    va_end(ap);
    return ret;
  }
  int addv(const char* fmt, va_list ap) {
    if (m_len < size()) {
      const int len = vsnprintf(m_buffer + m_len, size() - m_len, fmt, ap);
      if (len >= 0) {
        m_len += len;
      }
    }
    return m_len;
  }
  int set(const char* fmt, ...) {
    clear();
    va_list ap;
    va_start(ap, fmt);
    const int ret = addv(fmt, ap);
    va_end(ap);
    return ret;
  }
  void split(unsigned line_len) {
    if (line_len < 4) {
      return;
    }

    unsigned i = 1;
    unsigned start = 0;
    unsigned last = 0;

    while (true) {
      const char c = m_buffer[i];
      const int len = static_cast<int>(i) - start;
      const bool done = (c == 0 || len >= static_cast<int>(m_len));
      if (len > static_cast<int>(line_len)) {
        // Try to split.
        if (last > 0) {
          // We can split.
          m_buffer[last] = '\n';
          start = last + 1;
          last = 0;
          i = start + 1;
          continue;
        }
      }
      if (done) {
        return;
      }
      // Not ready to split.
      if (c == '\n') {
        start = i + 1;
        last = 0;
        i = start + 1;
        continue;
      }
      if (c == ' ') {
        // Mark where we can split.
        last = i;
      }
      i += 1;
    }
  }

 private:
  char m_buffer[LEN];
  unsigned m_len = 0;
};

}  // namespace og3
