cmake_minimum_required(VERSION 3.0) #may work with lower than 3.0 atm
project(WebCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(WARNINGS_FLAGS -pedantic -Wall -Wdouble-promotion -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2
        -Winit-self -Wlogical-op -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wduplicated-cond
        -Wsign-promo -Wstrict-null-sentinel -Wnull-dereference -Wundef -Werror -Wno-c++0x-compat -Wpointer-arith -Wnon-virtual-dtor
        -Wunreachable-code -Wunused)



include_directories(thirdparties/json/single_include/nlohmann)
add_subdirectory(WebCacheLib)
add_subdirectory(WebCacheCli)
if (BUILD_TESTING)
    add_subdirectory(WebCacheTests)
endif()
