// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/packet_reader.h"

#ifndef NATIVE
#include <CRC32.h>
#endif

#include <algorithm>
#include <cstring>

namespace og3::pkt {
namespace {
uint16_t getu16(const uint8_t* u) { return (static_cast<uint16_t>(u[0]) << 8) | u[1]; }
}  // namespace

PacketReader::PacketReader(const uint8_t* buffer, std::size_t nbytes)
    : m_buffer(buffer), m_buffer_size(nbytes) {}

PacketReader::ParseResult PacketReader::parse() {
  if (m_buffer_size < kHeaderSize) {
    return ParseResult::kBadSize;
  }
  if (m_buffer[0] != 'o' || m_buffer[1] != 'g' || m_buffer[2] != '3' || m_buffer[3] != 'p') {
    return ParseResult::kBadPrefix;
  }
  m_protocol_version_major = m_buffer[4];
  m_protocol_version_minor = m_buffer[5];
  // Assume that minor versions are forward/backward compatible.
  if (m_protocol_version_major != kProtocolVersionMajor) {
    return ParseResult::kBadVersion;
  }
  m_pkt_size = getu16(m_buffer + 6);
  if (m_pkt_size > m_buffer_size || m_pkt_size < 12) {
    return ParseResult::kBadSize;
  }
  m_seq_id = getu16(m_buffer + 8);
  m_num_msgs = getu16(m_buffer + 10);
  if (m_num_msgs & 0x8000) {
    if (m_pkt_size < (kHeaderSize + sizeof(uint32_t))) {
      return ParseResult::kBadSize;
    }
    m_has_crc = true;
    m_num_msgs &= 0x3FFF;
    const size_t crc_offset = m_pkt_size - sizeof(m_packet_crc);
    memcpy(&m_packet_crc, m_buffer + crc_offset, sizeof(m_packet_crc));
#ifndef NATIVE
    m_computed_crc = CRC32::calculate(m_buffer, crc_offset);
#else
    m_computed_crc = 0;
    for (size_t i = 0; i < crc_offset; i++) {
      m_computed_crc = (m_computed_crc << 8) | (m_computed_crc ^ m_buffer[i]);
    }
#endif
    if (m_packet_crc != m_computed_crc) {
      return ParseResult::kBadCrc;
    }
  }
  const int32_t msgs_size =
      static_cast<int32_t>(m_pkt_size) - kHeaderSize - (m_has_crc ? sizeof(sizeof(uint32_t)) : 0);
  if (static_cast<int32_t>(kMsgHeaderSize * m_num_msgs) > msgs_size) {
    return ParseResult::kBadSize;
  }
  m_num_available_msgs = std::min(kMaxAvailableMsgs, m_num_msgs);
  unsigned offset = kHeaderSize;
  for (unsigned i = 0; i < m_num_available_msgs; i++) {
    if (offset + kMsgHeaderSize > m_pkt_size) {
      return ParseResult::kBadSize;
    }
    m_msg_offsets[i] = offset;
    m_msg_sizes[i] = getu16(m_buffer + offset);
    if (offset + m_msg_sizes[i] > m_pkt_size) {
      return ParseResult::kBadSize;
    }
    // Start next message on 4-byte boundary.
    offset += 4 + (m_msg_sizes[i] + 3) / 4 * 4;
  }
  m_is_ok = true;
  return ParseResult::kOk;
}

bool PacketReader::get_msg(std::size_t msg_index, const uint8_t** out_msg, uint16_t* out_msg_type,
                           std::size_t* out_msg_size) const {
  if (msg_index >= m_num_msgs) {
    return false;
  }
  const uint8_t* msg_header = m_buffer + m_msg_offsets[msg_index];
  const uint8_t* msg = msg_header + kMsgHeaderSize;
  if (out_msg) {
    *out_msg = msg;
  }
  if (out_msg_type) {
    *out_msg_type = getu16(msg_header + 2);
  }
  if (out_msg_size) {
    *out_msg_size = m_msg_sizes[msg_index];
  }
  return true;
}

}  // namespace og3::pkt
