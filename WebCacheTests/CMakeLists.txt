project(WebCacheTests)
set(SOURCE_FILES main.cpp Mocks/HttpClientMock.h WebCacheTests.cpp HttpClientTests.cpp)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${WebCacheLib_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} WebCacheLib ${GTEST_LIBRARIES})
