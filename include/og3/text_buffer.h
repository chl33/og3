// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <stdarg.h>
#include <stdio.h>

#include "og3/compiler_definitions.h"

namespace og3 {

/**
 * @brief Base class for text buffer utilities.
 *
 * Provides a common interface for writing formatted text into a fixed-size buffer,
 * with support for splitting long lines.
 */
class TextBufferBase {
 public:
  /** @brief Constructs a TextBufferBase with a specific capacity. */
  explicit TextBufferBase(unsigned buffer_len) : m_buffer_len(buffer_len) {}

  /** @brief Resets the buffer to empty. */
  void clear() {
    m_len = 0;
    m_buffer[0] = 0;
  }

  /** @return The maximum capacity of the buffer. */
  unsigned size() const { return m_buffer_len; }
  /** @return The current length of the text in the buffer. */
  unsigned length() const { return m_len; }
  /** @return Constant pointer to the raw character buffer. */
  const char* text() const { return m_buffer; }

  /** @brief Appends a C-string to the buffer. */
  int add(const char* text);
  /** @brief Appends formatted text (printf-style) to the buffer. */
  int addf(const char* fmt, ...) OG3_PRINTF_FORMAT(2);
  /** @brief Appends formatted text (va_list) to the buffer. */
  int addv(const char* fmt, va_list ap);

  /** @brief Overwrites the buffer with a C-string. */
  int set(const char* text);
  /** @brief Overwrites the buffer with formatted text. */
  int setf(const char* fmt, ...) OG3_PRINTF_FORMAT(2);

  /**
   * @brief In-place replaces space characters with newlines to wrap text.
   * @param line_len The maximum allowed characters per line.
   */
  void split(unsigned line_len);

 protected:
  /** @brief Internal helper to set the memory backing the buffer. */
  void set_buffer(char* buffer) {
    m_buffer = buffer;
    clear();
  }

 private:
  char* m_buffer = nullptr;
  unsigned m_buffer_len = 0;
  unsigned m_len = 0;
};

/**
 * @brief A fixed-size text buffer with internal storage.
 * @tparam LEN The size of the internal buffer in bytes.
 */
template <unsigned LEN>
class TextBuffer : public TextBufferBase {
 public:
  /** @brief Constructs a TextBuffer. */
  TextBuffer() : TextBufferBase(LEN) { set_buffer(m_buffer_storage); }

 private:
  char m_buffer_storage[LEN];
};

}  // namespace og3
