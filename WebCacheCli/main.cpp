#include <iostream>
#include <chrono>
#include <fstream>
#include <json.hpp>
#include <WebCache.h>

using namespace Panda;
int main(int argc, char** argv)
{
  if(argc != 5)
  {
    std::cout << "cache content debug cli" << std::endl;
    std::cout << "usage   WebCacheCli input.json output.json refreshDelay delay " << std::endl;
    std::cout << "ctrl+c to quit" << std::endl;
    std::cout << R"( input.json format : [ { "server": "http://somewhere:8080/idontcare", "path" : "/potato/ws" ])" << std::endl;
    return -1;
  }

  std::string inputFile = argv[1];
  std::string outputFile = argv[2];
  std::chrono::seconds refreshdelay(atoi(argv[3]));
  std::chrono::milliseconds delaybetween(atoi(argv[4]));

  std::ifstream fin(inputFile);
  nlohmann::json config;
  fin >> config;

  WebCache cache;
  for(const auto& elem : config)
  {
    cache.addQuery({elem["server"], nlohmann::json::json_pointer(elem["path"])});
  }
  cache.start(refreshdelay, delaybetween);

  while (true)
  {
    std::this_thread::sleep_for(refreshdelay);
    std::ofstream of(outputFile);
    of << cache.raw();
  }
  return 0;
}