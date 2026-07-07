#include "query.hpp"
#include <gtest/gtest.h>

TEST(QueryBuilderTest, SerializesDomainNameCorrectly) {
  std::vector<uint8_t> query = build_query("google.com", 1);

  EXPECT_EQ(query[12], 6); // length of google
  EXPECT_EQ(query[13], 'g');
  EXPECT_EQ(query[19], 3); // length of com
  EXPECT_EQ(query[20], 'c');
  EXPECT_EQ(query[23], 0); // null terminator

  EXPECT_EQ(query[24], 0);
  EXPECT_EQ(query[25], 1); // qtype
  EXPECT_EQ(query[26], 0);
  EXPECT_EQ(query[27], 1); // qclass
}
