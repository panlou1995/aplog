cmake_minimum_required(VERSION 2.8.3)
project(aplog CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fno-omit-frame-pointer -g -std=c++14")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -std=c++14")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

include_directories(include)

add_library(aplog
src/logger.cxx
src/loggingevent.cxx
src/macro.cxx
src/time.cxx
src/appender.cxx
src/appenderfile.cxx
src/appendconsole.cxx
src/layout.cxx
src/threadpool.cxx
src/pollmanager.cxx
src/connection.cxx
src/connectionmanager.cxx
src/transporttcp.cxx
src/appendersocket.cxx
src/socketclientimpl.cxx
src/utils.cxx
src/aplogserver.cxx
src/appendersocketserver.cxx
src/utils.cxx
)

target_link_libraries(aplog ${catkin_LIBRARIES} libasan.so)
#target_link_libraries(aplog ${catkin_LIBRARIES})
