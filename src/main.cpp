#include "cli.hpp"
#include "resolver.hpp"
#include <print>

int main(int argc, char *argv[]) {

  auto cli = cli::parse(argc, argv);
  if (!cli) {
    return 1;
  }

  auto client = DnsClient::create(cli->server_ip);
  if (!client) {
    return 1;
  }

  auto payload = client->resolve(cli->domain_name);
  if (!payload) {
    return 1;
  }

  std::println("resolved ip: {}", payload->to_string());

  return 0;
}
