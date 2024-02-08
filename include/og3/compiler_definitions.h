// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#if defined(__GNUC__) && (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 4)) || \
    defined(__clang__)
#define RT3_PRINTF_FORMAT(X) __attribute__((format(printf, (X), 1 + (X))))
#define RT3_NONNULL(...) __attribute__((nonnull __VA_ARGS__))
#else
// No printf-style format checking for non-GCC compilers
#define RT3_PRINTF_FORMAT(X)
#define RT3_NONNULL(...)
#endif
