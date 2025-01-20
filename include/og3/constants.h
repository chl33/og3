// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <stdint.h>

namespace og3 {

constexpr long kThousand = 1000;
constexpr long kMillion = kThousand * kThousand;
constexpr long kBillion = kMillion * kThousand;

constexpr long kMsecInSec = kThousand;
constexpr long kUsecInSec = kMillion;
constexpr long kNsecInSec = kBillion;

constexpr long kUsecInMSec = kThousand;
constexpr long kNsecInMsec = kMillion;
constexpr long kNsecInUSec = kThousand;

constexpr long kSecInMin = 60;
constexpr long kMinInHour = 60;
constexpr long kHourInDay = 24;

constexpr long kSecInHour = kSecInMin * kMinInHour;
constexpr long kMinInDay = kMinInHour * kHourInDay;
constexpr long kSecInDay = kSecInMin * kMinInDay;

constexpr long kMsecInMin = kSecInMin * kMsecInSec;
constexpr long kMsecInHour = kSecInHour * kMsecInSec;
constexpr long kMsecInDay = kSecInDay * kMsecInSec;

constexpr int64_t kUsecInMin = static_cast<int64_t>(kSecInMin) * kUsecInSec;
constexpr int64_t kUsecInHour = static_cast<int64_t>(kSecInHour) * kUsecInSec;
constexpr int64_t kUsecInDay = static_cast<int64_t>(kSecInDay) * kUsecInSec;

constexpr int64_t kNsecInMin = static_cast<int64_t>(kSecInMin) * kNsecInSec;
constexpr int64_t kNsecInHour = static_cast<int64_t>(kSecInHour) * kNsecInSec;
constexpr int64_t kNsecInDay = static_cast<int64_t>(kSecInDay) * kNsecInSec;

}  // namespace og3
