#pragma once
#include <memory>
#include <chrono>
#include "HttpClient.h"
#include <json.hpp>
#include <mutex>
#include <list>
#include <deque>
#include <atomic>
#include <thread>

namespace Panda {

  class WebCache {
  public:
    WebCache(std::unique_ptr<IHttpClient> client = std::make_unique<HttpClient>());
    ~WebCache(){
      stop();
    _client = nullptr;
    }

    /**
     * Will stop the refresh, already fetched data are still available !
     * The current implementation may take a while to close depending on start arguments. (and it's fine for my use case)
     * A form of interruptable sleep would be nice, wait_for on condition_variable ?
     */
    void stop();

    /**
     * start the refresh loop
     * @param refreshInterval Time between the end of a refresh and the start of the next one
     * @param requestsDelay  Time between 2 requests in a refresh cycle (do not ddos the server)
     */
    void start(std::chrono::seconds refreshInterval, std::chrono::milliseconds requestsDelay);


    /**
     * Add a new url to the refresh loop (before or after starting).
     * If added after starting, and if a refresh cycle is in progress, this will be taken into account for the next refresh
     * @param webServer ex: http://127.0.0.1:8080/part_that_I_dont_want_in_my_cache/
     * @param uri ex: /end_of_the/url/I_want_to_keep
     */
    struct Address{
      std::string webServer;
      nlohmann::json::json_pointer pointer;
    };
    void addQuery(Address address);


    /**
     * read part of the cache
     * @return the requested value
     * @throw An exception if the element doesn't exists
     */
    nlohmann::json get(const nlohmann::json::json_pointer& pathInCache) const;


    /**
     * read part of the cache
     * @return the requested value or the default one if the value
     */
    template <class Type>
    Type get(const nlohmann::json::json_pointer& pathInCache, Type defaultValue) const;

    /**
     * @param cb a callback for error cases (url, error message, httpresponse).
     * The httpResponse isn't mandatory, it will be null if the server isn't responding or if the json isn't valid
     * @warning the callback function should not call any function from the cache
     */
    void setErrorCallback(std::function<void(const Address&, const std::string&, HttpResponse*)> cb);

    /**
     * @param refreshed a callback called when the cache is fully refreshed
     * @warning the callback function should not call any function from the cache
     */
    void setRefreshCallback(std::function<void()> refreshed);

    nlohmann::json raw() const;
  private:
    void run(std::chrono::seconds refreshInterval, std::chrono::milliseconds requestsDelay);
    void fetchOne();
    void fetch(const Address& fetched);
    void fail(const Address&, const std::string&, HttpResponse*);
    std::unique_ptr<IHttpClient> _client;
    std::function<void()> _refreshCb;
    std::function<void(const Address&, const std::string&, HttpResponse*)> _errorCb;
    nlohmann::json _cache;
    std::list<Address> _urlList;
    std::deque<Address> _urlQueue;
    std::atomic_bool _running;
    std::thread _runner;
    mutable std::mutex _mutex;

    void updateCache(const Address &fetched, HttpResponse &response);
  };
}