CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(network_info)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

SET(LIBPNET ${CMAKE_SOURCE_DIR}/../libpnet)
SET(LIBPNET_SOURCE ${LIBPNET}/src)
INCLUDE_DIRECTORIES(${LIBPNET}/include)

file(GLOB BASE_SOURCES "${LIBPNET_SOURCE}/*.cpp")
ADD_EXECUTABLE(get_all_netinfo ${BASE_SOURCES} get_all_netinfo.cpp)
