// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <cstddef>
#include <cstdint>

// This is a still-experimental multi-message packet format.
namespace og3::pkt {

// Packet:
// Magic              [4] 'og3p'
// ProtocolVersion    [2] major, minor
// PktSize            [2] uint16le, includes header
// SeqId              [2] uint16le
// NumMsgs            [2] uint16le
// - Msg 0
//   Sz               [2] uint16le, includes full msg
//   Type             [2] uint16le: indicates kind of data.
// -...

constexpr size_t kHeaderSize = 12;
constexpr uint8_t kProtocolVersionMajor = 0x0;
constexpr uint8_t kProtocolVersionMinor = 0x1;
constexpr size_t kMsgHeaderSize = 4;

}  // namespace og3::pkt
