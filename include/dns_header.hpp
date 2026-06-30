#pragma once

#include <cstdint>
#include <vector>

struct DnsHeader {
  /* unique id for this transaction */
  uint16_t id;
  /* flags and codes */
  uint16_t flags;
  /* number of question count (query domain count) */
  uint16_t qdcount;
  /* answer count */
  uint16_t ancount;
  /* name server count */
  uint16_t nscount;
  /* additional record count */
  uint16_t arcount;

  std::vector<uint8_t> serialize() const {
    std::vector<uint8_t> header;
    header.reserve(12);

    header.push_back(static_cast<uint8_t>(id >> 8));
    header.push_back(static_cast<uint8_t>(id & 0xFF));

    header.push_back(static_cast<uint8_t>(flags >> 8));
    header.push_back(static_cast<uint8_t>(flags & 0xFF));

    header.push_back(static_cast<uint8_t>(qdcount >> 8));
    header.push_back(static_cast<uint8_t>(qdcount & 0xFF));

    header.push_back(static_cast<uint8_t>(ancount >> 8));
    header.push_back(static_cast<uint8_t>(ancount & 0xFF));

    header.push_back(static_cast<uint8_t>(nscount >> 8));
    header.push_back(static_cast<uint8_t>(nscount & 0xFF));

    header.push_back(static_cast<uint8_t>(arcount >> 8));
    header.push_back(static_cast<uint8_t>(arcount & 0xFF));

    return header;
  }
};

