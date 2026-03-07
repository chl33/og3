// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include "og3/packet.h"

namespace og3::pkt {

/**
 * @brief Constructs multi-message binary packets.
 *
 * PacketWriter allows incremental building of packets in the 'og3p' format,
 * handling header updates and optional CRC32 calculation.
 */
class PacketWriter {
 public:
  /**
   * @brief Initializes a writer for an existing buffer.
   * @param buffer Target memory.
   * @param nbytes Size of the memory.
   * @param seq_num Sequence ID for the packet.
   */
  PacketWriter(uint8_t* buffer, std::size_t nbytes, uint16_t seq_num);

  /** @return true if the writer is in a valid state. */
  bool is_ok() const { return m_is_ok; }

  /**
   * @brief Appends a sub-message to the packet.
   * @param type Application-defined type ID.
   * @param msg Pointer to message data.
   * @param msg_size Size of message data.
   * @return true if message was added, false if no space remains.
   */
  bool add_message(uint16_t type, const uint8_t* msg, uint16_t msg_size);

  /** @return Total bytes written to the buffer. */
  uint16_t packet_size() const { return is_ok() ? m_pkt_size : 0; }

  /** @return true if a CRC32 has been added. */
  bool has_crc() { return 0x8000 & m_num_msgs; }

  /**
   * @brief Finalizes the packet by adding a CRC32 checksum.
   * No further messages can be added after calling this.
   * @return true if successful.
   */
  bool add_crc();

  /** @return Computed CRC32 value. */
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
