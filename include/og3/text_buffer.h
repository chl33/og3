// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <stdarg.h>
#include <stdio.h>

#include "og3/compiler_definitions.h"

namespace og3 {

// A utility for writing text into a buffer, and optionally splitting lines.
class TextBufferBase {
 public:
  explicit TextBufferBase(unsigned buffer_len) : m_buffer_len(buffer_len) {}
  void clear() {
    m_len = 0;
    m_buffer[0] = 0;
  }
  unsigned size() const { return m_buffer_len; }
  const char* text() const { return m_buffer; }
  int add(const char* text);
  int addf(const char* fmt, ...) OG3_PRINTF_FORMAT(2);
  int addv(const char* fmt, va_list ap);
  int set(const char* text);
  int setf(const char* fmt, ...) OG3_PRINTF_FORMAT(2);
  void split(unsigned line_len);

 protected:
  void set_buffer(char* buffer) {
    m_buffer = buffer;
    clear();
  }

 private:
  char* m_buffer = nullptr;
  unsigned m_buffer_len = 0;
  unsigned m_len = 0;
};

// TextBuffer with built-in storage.
template <unsigned LEN>
class TextBuffer : public TextBufferBase {
 public:
  TextBuffer() : TextBufferBase(LEN) { set_buffer(m_buffer_storage); }

 private:
  char m_buffer_storage[LEN];
};

}  // namespace og3
