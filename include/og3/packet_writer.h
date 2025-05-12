// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include "og3/packet.h"

// This is a still-experimental multi-message packet format.
namespace og3::pkt {

class PacketWriter {
 public:
  PacketWriter(uint8_t* buffer, std::size_t nbytes, uint16_t seq_num);
  bool is_ok() const { return m_is_ok; }
  bool add_message(uint16_t type, const uint8_t* msg, uint16_t msg_size);
  uint16_t packet_size() const { return is_ok() ? m_pkt_size : 0; }
  bool has_crc() { return 0x8000 & m_num_msgs; }
  // Add a crc at the end of the packet.
  // add_message() will not work on this packet after this.
  bool add_crc();
  uint32_t crc() const { return m_crc; }

 protected:
  uint8_t* m_buffer;
  const std::size_t m_buffer_size;
  bool m_is_ok = true;
  uint16_t m_pkt_size = 0;
  uint16_t m_num_msgs = 0;
  uint32_t m_crc = 0;
};

}  // namespace og3::pkt
