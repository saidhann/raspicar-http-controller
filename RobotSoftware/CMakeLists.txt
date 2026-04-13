cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MyHttpServer VERSION 1.0)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER "/usr/bin/aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/aarch64-linux-gnu-g++")

set(CMAKE_C_FLAGS "-mcpu=cortex-a53")
set(CMAKE_CXX_FLAGS "-mcpu=cortex-a53")

# cache flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags")
# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(LIBCAMERA_DIR "libcamera/libcamera-apps-prebuilts")
#set(CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH}:${CMAKE_SOURCE_DIR}/${LIBCAMERA_DIR}/")
#link_directories("${CMAKE_SOURCE_DIR}/${LIBCAMERA_DIR}/lib")
#include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/${LIBCAMERA_DIR}/include")
#set(LIBRARY_HINT "${CMAKE_SOURCE_DIR}/${LIBCAMERA_DIR}/lib/")

#set(LIBCAMERA_VERSION "0.0.0")
#set(LIBCAMERA_LINK_LIBRARIES "${CMAKE_SOURCE_DIR}/${LIBCAMERA_DIR}/lib/libcamera.so;${CMAKE_SOURCE_DIR}/${LIBCAMERA_DIR}/lib/libcamera-base.so")
#set(LIBCAMERA_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/${LIBCAMERA_DIR}/include/")

set(LDFLAGS "${LDFLAGS} -Wl,--no-as-needed")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed ")

#include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS})

# Add executable target
add_executable(MyHttpServer src/main.cpp)

# Include httplib header directory
target_include_directories(MyHttpServer PRIVATE ${LIBCAMERA_INCLUDE_DIRS})
target_include_directories(MyHttpServer PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/pifiles)
#target_include_directories(MyHttpServer PRIVATE ${OpenCV_INCLUDE_DIRS})
add_library( libgpio SHARED IMPORTED )
set_target_properties( libgpio PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/pifiles/libpigpio.so )
#target_link_libraries(MyHttpServer PRIVATE ${OpenCV_LIBS})
target_link_libraries(MyHttpServer PRIVATE ${LIBCAMERA_LINK_LIBRARIES} libgpio)

# Optional: If using HTTPS (OpenSSL), uncomment the following lines
# find_package(OpenSSL REQUIRED)
# target_link_libraries(MyHttpServer OpenSSL::SSL OpenSSL::Crypto)
