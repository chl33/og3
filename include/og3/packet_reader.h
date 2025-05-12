// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include "og3/packet.h"

// This is a still-experimental multi-message packet format.
namespace og3::pkt {

inline bool is_pkt(const uint8_t* buffer, std::size_t nbytes) {
  return nbytes >= kHeaderSize && buffer[0] == 'c' && buffer[1] == '3' &&
         buffer[2] == kProtocolVersionMajor && buffer[3] >= kProtocolVersionMinor;
}

class PacketReader {
 public:
  PacketReader(const uint8_t* buffer, std::size_t nbytes);

  enum ParseResult : int {
    kOk = 0,
    kBadSize = 1,
    kBadPrefix = 2,
    kBadVersion = 3,
    kBadCrc = 4,
  };
  static constexpr uint16_t kMaxAvailableMsgs = 16;

  ParseResult parse();

  bool get_msg(std::size_t msg_index, const uint8_t** out_msg, uint16_t* out_msg_type,
               std::size_t* out_msg_size) const;

  uint16_t buffer_size() const { return m_buffer_size; }
  bool is_ok() const { return m_is_ok; }
  bool has_crc() const { return m_has_crc; }
  uint8_t protocol_version_major() const { return m_protocol_version_major; }
  uint8_t protocol_version_minor() const { return m_protocol_version_minor; }
  uint16_t pkt_size() const { return m_pkt_size; }
  uint16_t seq_id() const { return m_seq_id; }
  uint16_t num_msgs() const { return m_num_msgs; }
  uint16_t num_available_msgs() const { return m_num_available_msgs; }
  uint32_t packet_crc() const { return m_packet_crc; }
  uint32_t computed_crc() const { return m_computed_crc; }

 protected:
  const uint8_t* m_buffer;
  const uint16_t m_buffer_size;
  bool m_is_ok = false;
  bool m_has_crc = false;
  uint8_t m_protocol_version_major = 0xff;
  uint8_t m_protocol_version_minor = 0xff;
  uint16_t m_pkt_size = 0;
  uint16_t m_seq_id = 0xff;
  uint16_t m_num_msgs = 0;
  uint16_t m_num_available_msgs = 0;
  uint16_t m_msg_sizes[kMaxAvailableMsgs];
  uint16_t m_msg_offsets[kMaxAvailableMsgs];
  uint32_t m_packet_crc = 0;
  uint32_t m_computed_crc = 0;
};

}  // namespace og3::pkt
