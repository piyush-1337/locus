#include "utils.hpp"
#include <cstdint>
#include <vector>

uint16_t read_uint16(const std::vector<uint8_t> &buffer, size_t &offset) {
  uint16_t value =
      (static_cast<uint16_t>(buffer[offset]) << 8) | buffer[offset + 1];
  offset += 2;
  return value;
}

uint32_t read_uint32(const std::vector<uint8_t> &buffer, size_t &offset) {
  uint32_t value = (static_cast<uint32_t>(buffer[offset]) << 24) |
                   (static_cast<uint32_t>(buffer[offset + 1]) << 16) |
                   (static_cast<uint32_t>(buffer[offset + 2]) << 8) |
                   buffer[offset + 3];
  offset += 4;
  return value;
}
