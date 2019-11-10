#include <gtest/gtest.h>
#include <BasicHttpClient.h>

using namespace Panda;
TEST(HttpClientTests, get_a_file)
{
  BasicHttpClient c;
  auto ret = c.get("http://whenwillyoulose.com");
  EXPECT_EQ(200, ret.code);
  EXPECT_EQ(9169, ret.message.size());
}

TEST(HttpClientTests, fail_a_connection) //this test requires to run on a machine with no http server on 666
{
  BasicHttpClient c;
  ASSERT_THROW(c.get("http://localhost:666"), HttpClientException);
}
