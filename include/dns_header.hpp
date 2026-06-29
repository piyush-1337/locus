#pragma once

#include <cstdint>
#include <vector>

struct DnsHeader {
  uint16_t id;
  uint16_t flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
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

