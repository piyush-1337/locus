#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <string>

struct Ipv4Addr {
  std::array<uint8_t, 4> octets;

  std::string to_string() const {
    return std::format("{}.{}.{}.{}", octets[0], octets[1], octets[2],
                       octets[3]);
  }
};

struct Ipv6Addr {
  std::array<uint8_t, 16> octets;

  std::string to_string() const {
    std::array<uint16_t, 8> blocks;
    for (int i = 0; i < 8; ++i) {
      blocks[i] = (octets[i * 2] << 8) | octets[i * 2 + 1];
    }

    int max_zero_start = -1;
    int max_zero_len = 0;

    int current_zero_start = -1;
    int current_zero_len = 0;

    for (int i = 0; i < 8; i++) {
      if (blocks[i] == 0) {
        if (current_zero_start == -1)
          current_zero_start = i;
        current_zero_len++;

        if (current_zero_len > max_zero_len) {
          max_zero_len = current_zero_len;
          max_zero_start = current_zero_start;
        }
      } else {
        current_zero_start = -1;
        current_zero_len = 0;
      }
    }

    if (max_zero_len == 1) {
      max_zero_len = 0;
      max_zero_start = -1;
    }

    std::string result;
    for (int i = 0; i < 8; ++i) {
      if (i == max_zero_start) {
        result += (i == 0) ? "::" : ":";

        i += max_zero_len - 1;
        continue;
      }

      result += std::format("{:x}", blocks[i]);

      if (i < 7 && i + 1 != max_zero_start) {
        result += ":";
      }
    }

    return result;
  }
};
