#include <iostream>
#include "BasicHttpClient.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#pragma GCC diagnostic pop
#include <cstring>
#include <sstream>

using namespace Panda;
using namespace std;


HttpResponse BasicHttpClient::get(const std::string& url) const {
  try
  {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;

    using namespace curlpp::Options;
    request.setOpt(Verbose(true));
    request.setOpt(Url(url));

    std::stringstream output;
    auto test = new curlpp::options::WriteStream(&output);
    request.setOpt(test);

    request.perform();

    HttpResponse r;
    r.code =  curlpp::infos::ResponseCode::get(request);
    r.message = output.str();
    return r;
  }
  catch ( curlpp::LogicError & e ) {
    throw HttpClientException(e.what()); //do something smarter
  }
  catch ( curlpp::RuntimeError & e ) {
    throw HttpClientException(e.what());//do something smarter
  }

}
