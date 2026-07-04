#pragma once

#include <optional>
#include <string_view>

struct CliConfig {
  std::string_view server_ip;
  std::string_view domain_name;
};

namespace cli {

std::optional<CliConfig> parse(int argc, char *argv[]);
void print_usage();

} // namespace cli
