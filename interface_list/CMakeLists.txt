CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(interface_list)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
SET(OUTPUT_ELF ${CMAKE_PROJECT_NAME})

INCLUDE_DIRECTORIES(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/../include)
LINK_DIRECTORIES(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/../lib)

SET(SOURCE_FILES interface_list.c)
ADD_EXECUTABLE(${OUTPUT_ELF} ${SOURCE_FILES})

IF(${USE_SHARED})
  TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} libpcap.so)
else()
  TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} libpcap.a)
endif()
