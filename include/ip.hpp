#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <string>

struct Ipv4Adrr {
  std::array<uint8_t, 4> octets;

  std::string to_string() const {
    return std::format("{}.{}.{}.{}", octets[0], octets[1], octets[2],
                       octets[3]);
  }
};
