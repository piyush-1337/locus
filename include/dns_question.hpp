#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct DnsQuestion {
  std::string_view domain;
  uint16_t qtype;
  uint16_t qclass;

  std::vector<uint8_t> serialize() const {
    std::vector<uint8_t> buffer;

    size_t start = 0;
    size_t dot_pos = domain.find('.');

    while (dot_pos != std::string::npos) {
      size_t length = dot_pos - start;
      std::string_view label(domain.data() + start, length);

      buffer.push_back(static_cast<uint8_t>(length));
      for (char c : label)
        buffer.push_back(c);

      start = dot_pos + 1;
      dot_pos = domain.find('.', start);
    }

    std::string_view label(domain.data() + start);
    buffer.push_back(static_cast<uint8_t>(label.size()));
    for (char c : label)
      buffer.push_back(c);

    buffer.push_back(0x00);

    // Qtype
    buffer.push_back(static_cast<uint8_t>(qtype >> 8));
    buffer.push_back(static_cast<uint8_t>(qtype & 0xFF));

    // Qclass
    buffer.push_back(static_cast<uint8_t>(qclass >> 8));
    buffer.push_back(static_cast<uint8_t>(qclass & 0xFF));

    return buffer;
  }
};
