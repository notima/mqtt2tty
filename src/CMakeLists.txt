cmake_minimum_required(VERSION 3.0.0)
project(Mqtt2Serial VERSION 0.1.0)

set(CMAKE_INSTALL_PREFIX /)

include(CTest)
enable_testing()

add_executable(Mqtt2Serial main.cpp)
install(TARGETS Mqtt2Serial
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
target_sources(Mqtt2Serial PRIVATE tty.cpp)
target_sources(Mqtt2Serial PRIVATE pty.cpp)
target_sources(Mqtt2Serial PRIVATE mqttClient.cpp)
target_sources(Mqtt2Serial PRIVATE mqttSubscriber.cpp)
target_link_libraries(Mqtt2Serial LINK_PUBLIC mosquitto util config log4cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
