#include "cli.hpp"
#include "resolver.hpp"

int main(int argc, char *argv[]) {

  auto domain_name = cli::parse(argc, argv);
  if (!domain_name) {
    return 1;
  }

  auto client = DnsClient::create();
  if (!client) {
    return 1;
  }

  return 0;
}
