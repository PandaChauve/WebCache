project(WebCacheCli)
set(SOURCE_FILES main.cpp main.cpp)

include_directories(${WebCacheLib_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} WebCacheLib)
