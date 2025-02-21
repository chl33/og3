// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/text_buffer.h"

namespace og3 {

int TextBufferBase::add(const char* text) { return addf("%s", text); }
int TextBufferBase::addf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  const int ret = addv(fmt, ap);
  va_end(ap);
  return ret;
}
int TextBufferBase::addv(const char* fmt, va_list ap) {
  if (m_len < size()) {
    const int len = vsnprintf(m_buffer + m_len, size() - m_len, fmt, ap);
    if (len >= 0) {
      m_len += len;
    }
  }
  return m_len;
}
int TextBufferBase::set(const char* text) {
  clear();
  return add(text);
}
int TextBufferBase::setf(const char* fmt, ...) {
  clear();
  va_list ap;
  va_start(ap, fmt);
  const int ret = addv(fmt, ap);
  va_end(ap);
  return ret;
}
void TextBufferBase::split(unsigned line_len) {
  if (line_len < 4) {
    return;
  }

  unsigned i = 1;
  unsigned start = 0;
  unsigned last = 0;

  while (true) {
    const char c = m_buffer[i];
    const int len = static_cast<int>(i) - start;
    const bool done = (c == 0 || len >= static_cast<int>(m_buffer_len));
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

}  // namespace og3
// namespace og3
