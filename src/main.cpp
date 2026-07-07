#include "cli.hpp"
#include "resolver.hpp"
#include <print>

int main(int argc, char *argv[]) {

  auto domain_name = cli::parse(argc, argv);
  if (!domain_name) {
    return 1;
  }

  auto client = DnsClient::create();
  if (!client) {
    return 1;
  }

  auto ip_addr_opt = client->resolve(*domain_name);
  if (!ip_addr_opt) {
    return 1;
  }

  auto ip_adrr = *ip_addr_opt;
  std::visit(
      [](const auto &ip) { std::println("resolved ip: {}", ip.to_string()); },
      ip_adrr);

  return 0;
}
