#include <iostream>
#include "WebCache.h"
using namespace std;
using namespace Panda;

WebCache::WebCache(std::unique_ptr<IHttpClient> client) :
    _client(std::move(client)),
    _refreshCb ([](){}),
    _errorCb([](const Address&, const std::string& message, HttpResponse*){ std::cerr << message << std::endl;}),
    _running(false)
{
  if(!_client) throw std::invalid_argument("client");
}

void WebCache::setErrorCallback(std::function<void(const Address &, const std::string&, HttpResponse *)> cb) {
  std::lock_guard<std::mutex> scopedLock(_mutex);
  _errorCb = std::move(cb);
}

void WebCache::setRefreshCallback(std::function<void()> refreshed) {
  std::lock_guard<std::mutex> scopedLock(_mutex);
  _refreshCb = std::move(refreshed);
}

void WebCache::stop() {
  _running = false;
  if(_runner.joinable())
    _runner.join();
}

void WebCache::start(std::chrono::seconds refreshInterval, std::chrono::milliseconds requestsDelay) {
  stop();
  _running = true;
  _runner = std::thread([&, refreshInterval, requestsDelay]()
                         {
                          run(refreshInterval, requestsDelay);
                         });
}

void WebCache::run(std::chrono::seconds refreshInterval, std::chrono::milliseconds requestsDelay) {

  while(_running)
  {
    {
      std::lock_guard<std::mutex> scopedLock(_mutex);
      _urlQueue = deque<Address>(_urlList.begin(), _urlList.end());
    }
    while(!_urlQueue.empty())
    {
      fetchOne(); //contains a lock !
      std::this_thread::sleep_for(requestsDelay);
    }
    {
      std::lock_guard<std::mutex> scopedLock(_mutex);
      _refreshCb();
    }
    std::this_thread::sleep_for(refreshInterval);
  }
}

void WebCache::fetchOne() {
  Address fetched = _urlQueue.front();
  _urlQueue.pop_front();
  fetch(fetched);
}

void WebCache::fetch(const WebCache::Address& fetched) {
  try
  {
    auto response =_client->get(fetched.webServer + fetched.pointer.to_string());
    if(response.code < 200 || response.code >= 400)
    {
      fail(fetched, "Http error", &response);
    }
    else
    {
      updateCache(fetched, response);
    }
  }
  catch(const exception& e)
  {
    fail(fetched, e.what(), nullptr);
  }
}

void WebCache::updateCache(const WebCache::Address &fetched, HttpResponse &response) {
  try {
    auto json = nlohmann::json::parse(response.message);
    lock_guard<mutex> scopedLock(_mutex); //this one is deep => be careful !
    _cache[fetched.pointer] = json;
  }
  catch(const nlohmann::json::exception& e)
  {
    fail(fetched, e.what(), &response);
  }
}

void WebCache::fail(const WebCache::Address& fetched, const std::string& message, HttpResponse* response) {

  std::lock_guard<std::mutex> scopedLock(_mutex);
  _errorCb(fetched, message, response);
}

void WebCache::addQuery(Address address) {
  std::lock_guard<std::mutex> scopedLock(_mutex);
  _urlList.emplace_back(std::move(address));
}

nlohmann::json WebCache::get(const nlohmann::json::json_pointer &pathInCache) const {
  std::lock_guard<std::mutex> scopedLock(_mutex);
  if(_cache.contains(pathInCache))
    return _cache[pathInCache];
  throw std::out_of_range("not in cache");
}

nlohmann::json WebCache::raw() const {
  std::lock_guard<std::mutex> scopedLock(_mutex);
  return _cache;
}
