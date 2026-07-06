#pragma once

#include "utils.hpp"
#include <cstdint>
#include <vector>

struct DnsRecordHeader {
  uint16_t type;
  uint16_t class_code;
  uint32_t ttl;
  uint16_t data_length;

  static DnsRecordHeader from_bytes(std::vector<uint8_t> &response, size_t &offset) {
    return DnsRecordHeader {
      .type = read_uint16(response, offset),
      .class_code = read_uint16(response, offset),
      .ttl = read_uint32(response, offset),
      .data_length = read_uint16(response, offset),
    };
  }
};
