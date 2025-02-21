// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#if defined(__GNUC__) && (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 4)) || \
    defined(__clang__)
#define OG3_PRINTF_FORMAT(X) __attribute__((format(printf, (X), 1 + (X))))
#else
// No printf-style format checking for non-GCC compilers
#define OG3_PRINTF_FORMAT(X)
#endif
