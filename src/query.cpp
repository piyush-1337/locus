#include "query.hpp"
#include "dns_header.hpp"
#include <string_view>

std::vector<uint8_t> build_query(std::string_view domain) {
  DnsHeader header{
      .id = 1234,
      .flags = static_cast<uint16_t>(0x0100),
      .qdcount = 1,
      .ancount = 0,
      .nscount = 0,
      .arcount = 0,
  };

  std::vector<uint8_t> query = header.serialize();

  size_t start = 0;
  size_t dot_pos = domain.find('.');

  while (dot_pos != std::string_view::npos) {
    size_t length = dot_pos - start;
    std::string_view label = domain.substr(start, length);

    start = dot_pos + 1;
    dot_pos = domain.find('.', start);

    query.push_back(static_cast<uint8_t>(length));
    for (char c : label) {
      query.push_back(c);
    }
  }

  std::string_view label = domain.substr(start);
  query.push_back(label.size());
  for (char c : label) {
    query.push_back(static_cast<uint8_t>(c));
  }

  query.push_back(static_cast<uint8_t>(0x00));

  // Qtype (00, 01 for ipv4, 00, 1C for ipv6)
  query.push_back(static_cast<uint8_t>(0x00));
  query.push_back(static_cast<uint8_t>(0x01));

  // Qclass
  query.push_back(static_cast<uint8_t>(0x00));
  query.push_back(static_cast<uint8_t>(0x01));

  return query;
}
