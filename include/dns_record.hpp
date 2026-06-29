#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct DnsRecord {
  std::string name;
  uint16_t type;
  uint16_t record_class;
  uint32_t ttl;
  std::vector<uint8_t> rdata;
};
