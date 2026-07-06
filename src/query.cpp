#include "query.hpp"
#include "dns_header.hpp"
#include "dns_question.hpp"
#include <string_view>

std::vector<uint8_t> build_query(std::string_view domain, uint16_t qtype) {
  DnsHeader header{
      .id = 1234,
      .flags = 0x0100,
      .qdcount = 1,
      .ancount = 0,
      .nscount = 0,
      .arcount = 0,
  };

  DnsQuestion question{
      .domain = domain, .qtype = qtype, .qclass = 1};

  std::vector<uint8_t> query = header.serialize();
  std::vector<uint8_t> q_bytes = question.serialize();

  query.insert(query.end(), q_bytes.begin(), q_bytes.end());

  return query;
}
