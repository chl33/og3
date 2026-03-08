// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <cstddef>
#include <cstdint>

/**
 * @brief Experimental multi-message binary packet format definitions.
 */
namespace og3::pkt {

/*
 * Packet Layout:
 * Magic              [4] 'og3p'
 * ProtocolVersion    [2] major, minor
 * PktSize            [2] uint16le (includes header)
 * SeqId              [2] uint16le
 * NumMsgs            [2] uint16le (MSB is CRC-present flag)
 *
 * - Message 0
 *   Sz               [2] uint16le (includes msg header)
 *   Type             [2] uint16le (application-defined)
 *   Data             [...]
 *
 * - ... (more messages)
 *
 * - (Optional CRC32) [4] uint32le
 */

constexpr size_t kHeaderSize = 12;              ///< @brief Packet header size.
constexpr uint8_t kProtocolVersionMajor = 0x0;  ///< @brief Current major version.
constexpr uint8_t kProtocolVersionMinor = 0x1;  ///< @brief Current minor version.
constexpr size_t kMsgHeaderSize = 4;            ///< @brief Sub-message header size.

}  // namespace og3::pkt
