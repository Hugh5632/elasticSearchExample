cmake_minimum_required(VERSION 3.17)
project(test_1)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)
set(INC_DIR
        /usr/local/include/curl
        lib/cpr_lib/include)
set(LINK_DIR /usr/local/lib
        lib/cpr_lib/lib)
set(ProgramSrc
        es_client.cc
        es_client.h
        es_client_impl.h
        es_bulk_api.cc
        es_bulk_api.h es_bulk_api_impl.h)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

## 动态库
#add_library(hx_es_shared SHARED ${ProgramSrc} )
#add_executable(hx_es_lib ${ProgramSrc})
#target_link_libraries(hx_es_lib hx_es_shared)
#
##静态库
#add_library(hx_es_static SHARED ${ProgramSrc} )
#add_executable(hx_es_so ${ProgramSrc})
#target_link_libraries(hx_es_so hx_es_static)

add_executable(test_1 main.cpp ${ProgramSrc})
target_link_libraries(
        test_1
#        hx_es_so
#        hx_es_lib
        CURL::libcurl
        OpenSSL::SSL
        libcpr.so
#        libgflags.a
#        libglog.a
        libmaxminddb.a
)