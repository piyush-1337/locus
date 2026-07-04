#include "cli.hpp"
#include "dns_header.hpp"
#include "query.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>
#include <vector>

int main(int argc, char *argv[]) {

  auto cli = cli::parse(argc, argv);
  if (!cli.has_value()) {
    return 1;
  }

  std::vector<uint8_t> query = build_query(cli->domain_name);

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    std::println(stderr, "Error: failed to create udp socket");
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_port = htons(53);
  server_addr.sin_family = AF_INET;

  std::string ip_str{cli->server_ip};

  if (inet_pton(AF_INET, ip_str.c_str(), &server_addr.sin_addr) <= 0) {
    std::println(stderr, "Error: invalid ip address");
    return 1;
  }

  ssize_t bytes_sent = sendto(sock, query.data(), query.size(), 0,
                              reinterpret_cast<const sockaddr *>(&server_addr),
                              sizeof(server_addr));
  if (bytes_sent == -1 || bytes_sent != query.size()) {
    std::println(stderr, "Error: coudn't send header through udp");
    return 1;
  }

  std::vector<uint8_t> response(512);
  ssize_t bytes_received =
      recvfrom(sock, response.data(), response.size(), 0, nullptr, nullptr);

  if (bytes_received == -1) {
    std::println(stderr, "Error: coudn't receive through udp");
    return 1;
  }

  response.resize(bytes_received);
  size_t offset = 0;
  DnsHeader reply_header;
  reply_header.id = read_uint16(response, offset);
  reply_header.flags = read_uint16(response, offset);
  reply_header.qdcount = read_uint16(response, offset);
  reply_header.ancount = read_uint16(response, offset);
  reply_header.nscount = read_uint16(response, offset);
  reply_header.arcount = read_uint16(response, offset);

  int response_code = reply_header.flags & 0x0F;
  if (reply_header.ancount == 0 || response_code == 3) {
    std::println(stderr, "no ip address found/doesn't exist for: {}",
                 cli->domain_name);
    return 1;
  }

  if (response_code != 0) {
    std::println(stderr, "query not successful");
    return 1;
  }

  // server just responds back the original query
  offset = query.size();
  offset += 10;

  uint16_t data_length = read_uint16(response, offset);
  if (data_length != 4) {
    std::println(stderr, "not ipv4 addr");
    return 1;
  }

  std::println("resolved ip address: {}.{}.{}.{}", response[offset],
               response[offset + 1], response[offset + 2],
               response[offset + 3]);

  return 0;
}
