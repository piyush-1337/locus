#include "resolver.hpp"
#include "dns_header.hpp"
#include "query.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <optional>
#include <print>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

std::optional<DnsClient> DnsClient::create(std::string_view server_ip) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    return std::nullopt;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(53);

  std::string ip_str{server_ip};
  if (inet_pton(AF_INET, ip_str.c_str(), &server_addr.sin_addr) <= 0) {
    close(sock);
    return std::nullopt;
  }

  return DnsClient(sock, server_addr);
}

std::optional<std::vector<uint8_t>>
DnsClient::resolve(std::string_view domain_name) {

  std::vector<uint8_t> query = build_query(domain_name);

  ssize_t bytes_sent =
      sendto(m_socket_fd, query.data(), query.size(), 0,
             reinterpret_cast<const sockaddr *>(&m_server_addr),
             sizeof(m_server_addr));

  if (bytes_sent == -1 || bytes_sent != query.size()) {
    std::println(stderr, "Error: coudn't send header through udp");
    return std::nullopt;
  }

  std::vector<uint8_t> response(512);
  ssize_t bytes_received = recvfrom(m_socket_fd, response.data(),
                                    response.size(), 0, nullptr, nullptr);

  if (bytes_received == -1) {
    std::println(stderr, "Error: coudn't receive through udp");
    return std::nullopt;
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
                 domain_name);
    return std::nullopt;
  }

  if (response_code != 0) {
    std::println(stderr, "query not successful");
    return std::nullopt;
  }

  // server just responds back the original query
  offset = query.size();
  offset += 10;

  uint16_t data_length = read_uint16(response, offset);
  if (data_length != 4) {
    std::println(stderr, "not ipv4 addr");
    return std::nullopt;
  }

  return std::vector<uint8_t>{response[offset], response[offset + 1],
                              response[offset + 2], response[offset + 3]};
}

DnsClient::DnsClient(int socket_fd, sockaddr_in addr)
    : m_socket_fd(socket_fd), m_server_addr(addr) {}

DnsClient::DnsClient(DnsClient &&other) noexcept
    : m_socket_fd(other.m_socket_fd), m_server_addr(other.m_server_addr) {
  other.m_socket_fd = -1;
}

DnsClient &DnsClient::operator=(DnsClient &&other) noexcept {
  if (this != &other) {
    if (m_socket_fd != -1) {
      close(m_socket_fd);
    }

    m_socket_fd = other.m_socket_fd;
    m_server_addr = other.m_server_addr;

    other.m_socket_fd = -1;
  }

  return *this;
}

DnsClient::~DnsClient() {
  if (m_socket_fd != -1) {
    close(m_socket_fd);
  }
}
