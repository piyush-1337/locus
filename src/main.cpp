#include "dns_header.hpp"
#include "query.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <print>
#include <string_view>
#include <sys/socket.h>
#include <vector>

void print_usage() {
  std::println(stderr, "{}", R"(
Usage: locus [OPTIONS]
Options:
  --server <IP>    Specify the target DNS server IPv4 address (e.g., 8.8.8.8)
  --domain <NAME>  The domain name to resolve (e.g., example.com)
                 )");
}

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::println(stderr, "Error: Missing required arguments.");
    print_usage();
    return 1;
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
      return 1;
    }
  }
  if (server_ip.empty() || domain_name.empty()) {
    std::println(stderr, "Error: Missing required arguments.");
    print_usage();
    return 1;
  }

  std::vector<uint8_t> query = build_query(domain_name);

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    std::println(stderr, "Error: failed to create udp socket");
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_port = htons(53);
  server_addr.sin_family = AF_INET;

  std::string ip_str{server_ip};

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

  return 0;
}
