#include <gtest/gtest.h>
#include <WebCache.h>
#include <WebCache.hpp>
#include <condition_variable>
#include "Mocks/HttpClientMock.h"

using namespace Panda;
using namespace std::chrono;
using namespace nlohmann;

class WebCacheTest : public ::testing::Test
{
public:
  const std::string WorkingUrl1 = "http://127.0.0.1/ws/42";
  const std::string WorkingUrl2 = "http://127.0.0.1/ws/666";
  const std::string InvalidJsonUrl = "http://127.0.0.1/ws/invalid";
  const std::string InvalidFailedToConnectUrl = "http://127.0.0.1/ws/nope";
  std::unique_ptr<HttpClientMock> createBaseTestClient()
  {
    auto mockClient = std::make_unique<HttpClientMock>();
    mockClient->callBack = [&](const std::string& url) -> HttpResponse
    {
      if(url == WorkingUrl1)
        return { 202, R"({"potato" : true})"};
      if(url == WorkingUrl2)
        return { 202, R"({"chips" : false})"};
      if(url == InvalidJsonUrl)
        return { 200, "this json is definitely valid"};
      if(url == InvalidFailedToConnectUrl)
        throw  HttpClientException("Nope");

      return {200, "{}"};
    };
    return mockClient;
  }
  WebCache::Address addressFromFullUrl(const std::string& add)
  {
    return { "http://127.0.0.1/ws", nlohmann::json::json_pointer(add.substr(strlen("http://127.0.0.1/ws")))};
  }

  void waitRefresh(WebCache& cache, int count)
  {
    std::mutex m;
    int counter = 0;
    std::condition_variable cv;
    bool ready = false;
    cache.setRefreshCallback([&](){
      counter++;
      if(counter == count) {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        cv.notify_all();
      }
    });

    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [&]{return ready;});
    cache.setRefreshCallback([](){});
  }
};

TEST_F(WebCacheTest, stop_can_be_called_at_any_point) //no assert this will just freeze when failing ;)
{
  WebCache tested(std::move(createBaseTestClient()));
  tested.stop();
  tested.start(std::chrono::seconds(0), std::chrono::milliseconds(0));
  tested.stop();
}

TEST_F(WebCacheTest, the_cache_wont_call_errorcb_without_reason)
{
  WebCache tested(std::move(createBaseTestClient()));
  tested.setErrorCallback([](const WebCache::Address&, const std::string& message, HttpResponse*)
                          {
                            ASSERT_FALSE(true) << message;
                          });
  tested.addQuery(addressFromFullUrl(WorkingUrl1));
  tested.addQuery(addressFromFullUrl(WorkingUrl2));
  tested.start(seconds(0), milliseconds(0));
  waitRefresh(tested, 3);

}

TEST_F(WebCacheTest, we_can_use_default_values)
{
  WebCache tested(std::move(createBaseTestClient()));
  EXPECT_EQ(42, tested.get<int>("/666/chips"_json_pointer, 42));
}
TEST_F(WebCacheTest, well_get_an_exception_if_data_are_not_Available)
{
  WebCache tested(std::move(createBaseTestClient()));
  ASSERT_ANY_THROW(tested.get("/666/chips"_json_pointer));
}

TEST_F(WebCacheTest, the_cache_will_read_data_from_the_server)
{
  WebCache tested(std::move(createBaseTestClient()));
  tested.addQuery(addressFromFullUrl(WorkingUrl1));
  tested.addQuery(addressFromFullUrl(WorkingUrl2));
  tested.start(seconds(0), milliseconds(0));
  waitRefresh(tested, 1);
  EXPECT_EQ(true, tested.get("/42/potato"_json_pointer).get<bool>());
  EXPECT_EQ(false, tested.get<bool>("/666/chips"_json_pointer, true));
}

TEST_F(WebCacheTest, the_cache_will_call_the_error_callback_with_invalid_json)
{
  WebCache tested(std::move(createBaseTestClient()));
  tested.addQuery(addressFromFullUrl(InvalidJsonUrl));
  bool called = false;
  tested.setErrorCallback([&](const WebCache::Address&, const std::string& message, HttpResponse* r)
                          {
                            ASSERT_TRUE(r);
                            called = true;
                          });
  tested.start(seconds(0), milliseconds(0));
  waitRefresh(tested, 1);
  ASSERT_TRUE(called);
  tested.stop();
}
TEST_F(WebCacheTest, the_cache_will_call_the_error_callback_for_failing_servers)
{
  WebCache tested(std::move(createBaseTestClient()));
  tested.addQuery(addressFromFullUrl(InvalidFailedToConnectUrl));
  bool called = false;
  tested.setErrorCallback([&](const WebCache::Address&, const std::string& message, HttpResponse* r)
                          {
                            ASSERT_FALSE(r);
                            called = true;
                          });
  tested.start(seconds(0), milliseconds(0));
  waitRefresh(tested, 1);
  ASSERT_TRUE(called);
  tested.stop();
}

TEST_F(WebCacheTest, the_cache_will_read_data_from_the_server_even_if_some_routes_are_fucked)
{
  WebCache tested(std::move(createBaseTestClient()));
  tested.addQuery(addressFromFullUrl(WorkingUrl1));
  tested.addQuery(addressFromFullUrl(WorkingUrl2));
  tested.addQuery(addressFromFullUrl(InvalidJsonUrl));
  tested.addQuery(addressFromFullUrl(InvalidFailedToConnectUrl));
  tested.start(seconds(0), milliseconds(0));
  waitRefresh(tested, 1);
  EXPECT_EQ(true, tested.get("/42/potato"_json_pointer).get<bool>());
  EXPECT_EQ(false, tested.get<bool>("/666/chips"_json_pointer, true));

}