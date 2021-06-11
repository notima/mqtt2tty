cmake_minimum_required(VERSION 3.0.0)
project(Mqtt2Serial VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(Mqtt2Serial main.cpp)
target_sources(Mqtt2Serial PRIVATE tty.cpp)
target_sources(Mqtt2Serial PRIVATE mqttClient.cpp)
target_sources(Mqtt2Serial PRIVATE mqttSubscriber.cpp)
target_link_libraries(Mqtt2Serial LINK_PUBLIC mosquitto jansson util config)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
