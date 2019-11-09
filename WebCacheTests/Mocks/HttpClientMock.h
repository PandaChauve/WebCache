#pragma once


#include <HttpClient.h>
#include <functional>

namespace Panda {
  class HttpClientMock : public IHttpClient
  {
  public:
    std::function<HttpResponse(const std::string&)> callBack;
    HttpResponse get(const std::string& url) const override
    {
      return callBack(url);
    };
  };
}