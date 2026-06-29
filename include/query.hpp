#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

std::vector<uint8_t> build_query(std::string_view domain);
