project(WebCacheLib)
add_definitions(${WARNINGS_FLAGS})

set(SOURCE_FILES WebCache.cpp WebCache.h WebCache.hpp HttpClient.cpp HttpClient.h)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
