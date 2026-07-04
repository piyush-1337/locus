#pragma once

#include "ip.hpp"
#include <netinet/in.h>
#include <optional>
#include <string_view>

class DnsClient {
public:
  static std::optional<DnsClient> create(std::string_view server_ip);

  ~DnsClient();

  DnsClient(const DnsClient&) = delete;
  DnsClient& operator=(const DnsClient&) = delete;

  DnsClient(DnsClient&& other) noexcept;
  DnsClient& operator=(DnsClient&& other) noexcept;

  std::optional<Ipv4Adrr> resolve(std::string_view domain_name);

private:
  explicit DnsClient(int socket_fd, sockaddr_in addr);

  int m_socket_fd;
  sockaddr_in m_server_addr;
};
