#pragma once

#include <cstdint>
#include <stdexcept>

namespace Panda {
  typedef uint16_t HttpCode;
  struct HttpResponse
  {
    HttpCode code;
    std::string message;
  };

  class HttpClientException : public std::runtime_error{
    using runtime_error::runtime_error;
  };
  class IHttpClient {
  public:
    virtual ~IHttpClient() = default;
    virtual HttpResponse get(const std::string& url) const = 0;
  };

  class BasicHttpClient : public IHttpClient {
  public:
    HttpResponse get(const std::string& url) const override;
  };

}