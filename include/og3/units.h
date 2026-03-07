// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

/**
 * @brief Predefined unit strings compatible with Home Assistant.
 */
namespace og3::units {

// Unit name strings which match the set recognized by Home Assistant.
// https://github.com/home-assistant/core/blob/master/homeassistant/const.py
extern const char kWatt[];               ///< "W"
extern const char kKiloWatt[];           ///< "kW"
extern const char kVolt[];               ///< "V"
extern const char kWattHour[];           ///< "Wh"
extern const char kKiloWattHour[];       ///< "kWh"
extern const char kAmpere[];             ///< "A"
extern const char kDegree[];             ///< "°"
extern const char kCelsius[];            ///< "°C"
extern const char kFahrenheit[];         ///< "°F"
extern const char kKelvin[];             ///< "K"
extern const char kMicroseconds[];       ///< "μs"
extern const char kMilliseconds[];       ///< "ms"
extern const char kSeconds[];            ///< "s"
extern const char kMinutes[];            ///< "min"
extern const char kMillimeters[];        ///< "mm"
extern const char kMeters[];             ///< "m"
extern const char kPascals[];            ///< "Pa"
extern const char kHPascals[];           ///< "hPa"
extern const char kPsi[];                ///< "psi"
extern const char kGrams[];              ///< "g"
extern const char kKilograms[];          ///< "kg"
extern const char kPounds[];             ///< "lb"
extern const char kPercentage[];         ///< "%"
extern const char kMetersPerSecond[];    ///< "m/s"
extern const char kKilometersPerHour[];  ///< "km/h"
extern const char kMilesPerHour[];       ///< "mph"
extern const char kDecibel[];            ///< "dB"
extern const char kKilobytes[];          ///< "kB"
extern const char kMegabytes[];          ///< "MB"

}  // namespace og3::units
