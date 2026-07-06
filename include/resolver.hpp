#pragma once

#include "ip.hpp"
#include <netinet/in.h>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>

class DnsClient {
public:
  [[deprecated("old")]]
  static std::optional<DnsClient> create_old(std::string_view server_ip);
  static std::optional<DnsClient> create();

  ~DnsClient();

  DnsClient(const DnsClient &) = delete;
  DnsClient &operator=(const DnsClient &) = delete;

  DnsClient(DnsClient &&other) noexcept;
  DnsClient &operator=(DnsClient &&other) noexcept;

  [[deprecated("old")]]
  std::optional<std::variant<Ipv4Addr, Ipv6Addr>>
  resolve_old(std::string_view domain_name);

  std::optional<std::variant<Ipv4Addr, Ipv6Addr>>
  resolve(std::string_view domain_name);

  bool send_packet(const std::vector<uint8_t> &packet);
  std::optional<std::vector<uint8_t>> receive_packet();

private:
  explicit DnsClient(int socket_fd, sockaddr_in addr);

  int m_socket_fd;
  sockaddr_in m_server_addr;
};
