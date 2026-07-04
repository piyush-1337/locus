#include "cli.hpp"
#include <optional>
#include <print>

namespace cli {
std::optional<CliConfig> parse(int argc, char *argv[]) {

  if (argc < 5) {
    std::println(stderr, "Error: Missing required arguments.");
    print_usage();
    return std::nullopt;
  }

  std::string_view server_ip{};
  std::string_view domain_name{};

  for (int i{1}; i < argc; i++) {
    std::string_view arg{argv[i]};

    if (arg == "--server") {
      server_ip = argv[i + 1];
      i++;
    } else if (arg == "--domain") {
      domain_name = argv[i + 1];
      i++;
    } else {
      std::println(stderr, "Error: Unknown argument '{}'", arg);
      print_usage();
      return std::nullopt;
    }
  }
  if (server_ip.empty() || domain_name.empty()) {
    std::println(stderr, "Error: Missing required arguments.");
    print_usage();
    return std::nullopt;
  }

  return CliConfig{
      .server_ip = server_ip,
      .domain_name = domain_name,
  };
}

void print_usage() {
  std::println(stderr, "{}", R"(
Usage: locus [OPTIONS]
Options:
  --server <IP>    Specify the target DNS server IPv4 address (e.g., 8.8.8.8)
  --domain <NAME>  The domain name to resolve (e.g., example.com)
                 )");
}

} // namespace cli
