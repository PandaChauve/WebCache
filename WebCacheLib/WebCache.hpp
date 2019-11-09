#pragma once

#include <json.hpp>
#include "WebCache.h"

namespace Panda
{
  template <class Type>
  Type WebCache::get(const nlohmann::json::json_pointer& uri, Type defaultValue) const
  {
    std::lock_guard<std::mutex> scopedLock(_mutex);
    if(_cache.contains(uri)) //FIXME still missing type checking
      return _cache[uri].get<Type>();
    return defaultValue;
  }
}