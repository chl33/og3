// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <stdint.h>

namespace og3 {

constexpr long kThousand = 1000;                  ///< One thousand.
constexpr long kMillion = kThousand * kThousand;  ///< One million.
constexpr long kBillion = kMillion * kThousand;   ///< One billion.

constexpr long kMsecInSec = kThousand;  ///< Milliseconds in a second.
constexpr long kUsecInSec = kMillion;   ///< Microseconds in a second.
constexpr long kNsecInSec = kBillion;   ///< Nanoseconds in a second.

constexpr long kUsecInMSec = kThousand;  ///< Microseconds in a millisecond.
constexpr long kNsecInMsec = kMillion;   ///< Nanoseconds in a millisecond.
constexpr long kNsecInUSec = kThousand;  ///< Nanoseconds in a microsecond.

constexpr long kSecInMin = 60;   ///< Seconds in a minute.
constexpr long kMinInHour = 60;  ///< Minutes in an hour.
constexpr long kHourInDay = 24;  ///< Hours in a day.

constexpr long kSecInHour = kSecInMin * kMinInHour;  ///< Seconds in an hour.
constexpr long kMinInDay = kMinInHour * kHourInDay;  ///< Minutes in a day.
constexpr long kSecInDay = kSecInMin * kMinInDay;    ///< Seconds in a day.

constexpr long kMsecInMin = kSecInMin * kMsecInSec;    ///< Milliseconds in a minute.
constexpr long kMsecInHour = kSecInHour * kMsecInSec;  ///< Milliseconds in an hour.
constexpr long kMsecInDay = kSecInDay * kMsecInSec;    ///< Milliseconds in a day.

constexpr int64_t kUsecInMin =
    static_cast<int64_t>(kSecInMin) * kUsecInSec;  ///< Microseconds in a minute.
constexpr int64_t kUsecInHour =
    static_cast<int64_t>(kSecInHour) * kUsecInSec;  ///< Microseconds in an hour.
constexpr int64_t kUsecInDay =
    static_cast<int64_t>(kSecInDay) * kUsecInSec;  ///< Microseconds in a day.

constexpr int64_t kNsecInMin =
    static_cast<int64_t>(kSecInMin) * kNsecInSec;  ///< Nanoseconds in a minute.
constexpr int64_t kNsecInHour =
    static_cast<int64_t>(kSecInHour) * kNsecInSec;  ///< Nanoseconds in an hour.
constexpr int64_t kNsecInDay =
    static_cast<int64_t>(kSecInDay) * kNsecInSec;  ///< Nanoseconds in a day.

}  // namespace og3
