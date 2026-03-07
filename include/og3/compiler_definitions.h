// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#if defined(__GNUC__) && (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 4)) || \
    defined(__clang__)
/**
 * @brief Macro for printf-style format checking in GCC and Clang.
 * @param X The 1-based index of the format string parameter.
 */
#define OG3_PRINTF_FORMAT(X) __attribute__((format(printf, (X), 1 + (X))))
#else
/**
 * @brief Macro for printf-style format checking (empty for non-GCC/Clang).
 * @param X The 1-based index of the format string parameter.
 */
#define OG3_PRINTF_FORMAT(X)
#endif
