#include "resolver.hpp"
#include "dns_header.hpp"
#include "dns_record.hpp"
#include "ip.hpp"
#include "query.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <netinet/in.h>
#include <optional>
#include <print>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <variant>

std::optional<DnsClient> DnsClient::create() {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    return std::nullopt;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(53);

  std::string ip_str{"198.41.0.4"};
  if (inet_pton(AF_INET, ip_str.c_str(), &server_addr.sin_addr) <= 0) {
    close(sock);
    return std::nullopt;
  }

  return DnsClient(sock, server_addr);
}

std::optional<std::variant<Ipv4Addr, Ipv6Addr>>
DnsClient::resolve(std::string_view domain_name) {
  auto query = build_query(domain_name);
  int max_hops = 30;

  while (max_hops--) {
    if (!send_packet(query))
      return std::nullopt;

    auto packet_opt = receive_packet();
    if (!packet_opt) {
      return std::nullopt;
    }

    auto response = *packet_opt;
    size_t offset = 0;

    DnsHeader reply_header = DnsHeader::from_bytes(response, offset);
    int response_code = reply_header.flags & 0x0F;
    if (response_code != 0) {
      std::println(stderr, "query failed with response code: {}",
                   response_code);
      return std::nullopt;
    }

    // skip the echoed part
    offset = query.size();

    // answer section
    for (int i{}; i < reply_header.ancount; i++) {
      skip_name(response, offset);

      DnsRecordHeader record_header =
          DnsRecordHeader::from_bytes(response, offset);

      if (record_header.type == 1) {
        return Ipv4Addr{
            response[offset],
            response[offset + 1],
            response[offset + 2],
            response[offset + 3],
        };
      } else if (record_header.type == 28) {
        return Ipv6Addr{
            response[offset],      response[offset + 1],  response[offset + 2],
            response[offset + 3],  response[offset + 4],  response[offset + 5],
            response[offset + 6],  response[offset + 7],  response[offset + 8],
            response[offset + 9],  response[offset + 10], response[offset + 11],
            response[offset + 12], response[offset + 13], response[offset + 14],
            response[offset + 15],
        };
      }

      // skip everything else
      offset += record_header.data_length;
    }

    // authority section, jusk skip the names
    for (int i{}; i < reply_header.nscount; i++) {
      skip_name(response, offset);

      DnsRecordHeader record_header =
          DnsRecordHeader::from_bytes(response, offset);

      offset += record_header.data_length;
    }

    // additional section
    bool ip_found = false;
    for (int i{}; i < reply_header.arcount; i++) {
      skip_name(response, offset);

      DnsRecordHeader record_header =
          DnsRecordHeader::from_bytes(response, offset);

      // found ipv4 for next hop
      if (record_header.type == 1) {
        ip_found = true;

        auto ip = Ipv4Addr{
            response[offset],
            response[offset + 1],
            response[offset + 2],
            response[offset + 3],
        };

        uint32_t raw_ip = ip.raw();
        m_server_addr.sin_addr.s_addr = htonl(raw_ip);

        std::println("jumping to new server: {}", ip.to_string());
        break;
      }

      // try the next header for ip
      offset += record_header.data_length;
    }

    if (ip_found) {
      continue;
    }

    std::println(stderr, "resolution failed. no ip found");
    return std::nullopt;
  }

  std::println(stderr, "resolution failed. max hops reached");
  return std::nullopt;
}

std::optional<DnsClient> DnsClient::create_old(std::string_view server_ip) {
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

std::optional<std::variant<Ipv4Addr, Ipv6Addr>>
DnsClient::resolve_old(std::string_view domain_name) {

  std::vector<uint8_t> query = build_query(domain_name);

  bool sent = send_packet(query);
  if (!sent) {
    return std::nullopt;
  }

  auto packet = receive_packet();
  if (!packet) {
    return std::nullopt;
  }

  auto response = *packet;
  size_t offset = 0;

  DnsHeader reply_header = DnsHeader::from_bytes(response, offset);

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

  // answer section begins
  for (int i = 0; i < reply_header.ancount; i++) {
    // skip name
    skip_name(response, offset);

    DnsRecordHeader record_header =
        DnsRecordHeader::from_bytes(response, offset);

    if (record_header.type == 1) {
      return Ipv4Addr{{
          response[offset],
          response[offset + 1],
          response[offset + 2],
          response[offset + 3],
      }};
    } else if (record_header.type == 28) {
      return Ipv6Addr{{
          response[offset],
          response[offset + 1],
          response[offset + 2],
          response[offset + 3],
          response[offset + 4],
          response[offset + 5],
          response[offset + 6],
          response[offset + 7],
          response[offset + 8],
          response[offset + 9],
          response[offset + 10],
          response[offset + 11],
          response[offset + 12],
          response[offset + 13],
          response[offset + 14],
          response[offset + 15],
      }};
    }

    // else we dont care, gimme ipv4 | ipv6
    offset += record_header.data_length;
  }

  std::println("ip not found, possibly parsing error");
  return std::nullopt;
}

bool DnsClient::send_packet(const std::vector<uint8_t> &packet) {
  ssize_t bytes_sent =
      sendto(m_socket_fd, packet.data(), packet.size(), 0,
             reinterpret_cast<const sockaddr *>(&m_server_addr),
             sizeof(m_server_addr));

  if (bytes_sent == -1 || bytes_sent != packet.size()) {
    std::println(stderr, "Error: coudn't send header through udp");
    return false;
  }

  return true;
}

std::optional<std::vector<uint8_t>> DnsClient::receive_packet() {
  std::vector<uint8_t> response(512);
  ssize_t bytes_received = recvfrom(m_socket_fd, response.data(),
                                    response.size(), 0, nullptr, nullptr);

  if (bytes_received == -1) {
    std::println(stderr, "Error: coudn't receive through udp");
    return std::nullopt;
  }
  response.resize(bytes_received);

  return response;
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
