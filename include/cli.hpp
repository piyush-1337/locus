#pragma once

#include <optional>
#include <string_view>

namespace cli {

std::optional<std::string_view> parse(int argc, char *argv[]);
void print_usage();

} // namespace cli
