#pragma once

#include <cstdint>
#include <vector>

uint16_t read_uint16(const std::vector<uint8_t>& buffer, size_t& offset);

uint32_t read_uint32(const std::vector<uint8_t>& buffer, size_t& offset);

void print_usage();
