CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
project (sik-radio)

add_executable(sikradio-receiver receiver.cc)
add_executable(sikradio-sender sender.cc options.cc const.h)


FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(".")

TARGET_LINK_LIBRARIES(sikradio-receiver ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(sikradio-sender ${Boost_LIBRARIES})
