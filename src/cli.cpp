#include "cli.hpp"
#include <optional>
#include <print>
#include <string_view>

namespace cli {
std::optional<std::string_view> parse(int argc, char *argv[]) {

  if (argc < 3) {
    std::println(stderr, "Error: Missing required arguments.");
    print_usage();
    return std::nullopt;
  }

  std::string_view domain_name{};

  for (int i{1}; i < argc; i++) {
    std::string_view arg{argv[i]};

    if (arg == "--domain") {
      domain_name = argv[i + 1];
      i++;
    } else {
      std::println(stderr, "Error: Unknown argument '{}'", arg);
      print_usage();
      return std::nullopt;
    }
  }

  if (domain_name.empty()) {
    std::println(stderr, "Error: Missing required arguments.");
    print_usage();
    return std::nullopt;
  }

  return domain_name;
}

void print_usage() {
  std::println(stderr, "{}", R"(
Usage: locus [OPTIONS]
Options:
  --domain <NAME>  The domain name to resolve (e.g., example.com)
                 )");
}

} // namespace cli
