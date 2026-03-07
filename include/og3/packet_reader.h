// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include "og3/packet.h"

namespace og3::pkt {

/**
 * @brief Checks if a buffer contains a valid og3 packet header.
 * @param buffer Pointer to the raw data.
 * @param nbytes Size of the buffer.
 * @return true if the magic prefix and version major match.
 */
inline bool is_pkt(const uint8_t* buffer, std::size_t nbytes) {
  return nbytes >= kHeaderSize && buffer[0] == 'c' && buffer[1] == '3' &&
         buffer[2] == kProtocolVersionMajor && buffer[3] >= kProtocolVersionMinor;
}

/**
 * @brief Parses multi-message binary packets.
 *
 * PacketReader decodes the structured 'og3p' format, validating the header,
 * payload size, and optional CRC32. It provides access to individual sub-messages.
 */
class PacketReader {
 public:
  /** @brief Constructs a reader for a specific data buffer. */
  PacketReader(const uint8_t* buffer, std::size_t nbytes);

  /** @brief Error codes for packet parsing. */
  enum ParseResult : int {
    kOk = 0,          ///< Success.
    kBadSize = 1,     ///< Declared packet size exceeds buffer.
    kBadPrefix = 2,   ///< Magic prefix mismatch.
    kBadVersion = 3,  ///< Protocol major version mismatch.
    kBadCrc = 4,      ///< CRC32 validation failed.
  };
  static constexpr uint16_t kMaxAvailableMsgs = 16;  ///< @brief Internal limit for sub-messages.

  /**
   * @brief Performs validation and indexing of the packet content.
   * @return ParseResult indicating success or specific failure.
   */
  ParseResult parse();

  /**
   * @brief Retrieves a specific sub-message by index.
   * @param msg_index Index from 0 to num_available_msgs() - 1.
   * @param out_msg Pointer to the sub-message data.
   * @param out_msg_type Type code of the sub-message.
   * @param out_msg_size Size of the sub-message data.
   * @return true if the message was found.
   */
  bool get_msg(std::size_t msg_index, const uint8_t** out_msg, uint16_t* out_msg_type,
               std::size_t* out_msg_size) const;

  /** @return Capacity of the input buffer. */
  uint16_t buffer_size() const { return m_buffer_size; }
  /** @return true if parse() was successful. */
  bool is_ok() const { return m_is_ok; }
  /** @return true if the packet header indicates a CRC is present. */
  bool has_crc() const { return m_has_crc; }
  /** @return Major protocol version. */
  uint8_t protocol_version_major() const { return m_protocol_version_major; }
  /** @return Minor protocol version. */
  uint8_t protocol_version_minor() const { return m_protocol_version_minor; }
  /** @return Declared packet size from header. */
  uint16_t pkt_size() const { return m_pkt_size; }
  /** @return Packet sequence ID. */
  uint16_t seq_id() const { return m_seq_id; }
  /** @return Number of sub-messages declared in header. */
  uint16_t num_msgs() const { return m_num_msgs; }
  /** @return Number of sub-messages successfully indexed. */
  uint16_t num_available_msgs() const { return m_num_available_msgs; }
  /** @return CRC32 value read from the packet. */
  uint32_t packet_crc() const { return m_packet_crc; }
  /** @return Locally computed CRC32 of the received payload. */
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
