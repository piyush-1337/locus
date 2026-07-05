#pragma once

#include <cstdint>

struct DnsRecordHeader {
  uint16_t type;
  uint16_t class_code;
  uint32_t ttl;
  uint16_t data_length;
};
