#include "cli.hpp"
#include "resolver.hpp"
#include <print>

int main(int argc, char *argv[]) {

  auto cli = cli::parse(argc, argv);
  if (!cli.has_value()) {
    return 1;
  }


  auto client = DnsClient::create(cli->server_ip);
  if (!client.has_value()) {
    return 1;
  }

  auto payload = client->resolve(cli->domain_name);
  if (!payload.has_value()) {
    return 1;
  }

  std::println("resolved ip: {}", payload.value());

  return 0;
}
