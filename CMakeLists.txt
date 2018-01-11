cmake_minimum_required (VERSION 3.10)
project (__REPLACE__)
# The version number.
set (app_VERSION_MAJOR 1)
set (app_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)


 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/app_config.h.in"
  "${PROJECT_BINARY_DIR}/app_config.h"
  )
 
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

# add the binary tree to the search path for include files
# so that we will find app_config.h
include_directories("${PROJECT_BINARY_DIR}")

# add the executable
add_library(__REPLACE__-lib server.cc http.cc)
add_executable(__REPLACE__ main.cc)

find_package(Folly REQUIRED)
find_package(Wangle REQUIRED)
find_package(gflags REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Glog REQUIRED)
find_package(Folly REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread filesystem regex context)
find_package(OpenSSL REQUIRED)
find_package(Glog REQUIRED)
find_package(Gflags REQUIRED)
find_package(Libevent REQUIRED)
find_package(DoubleConversion REQUIRED)
find_package(Threads REQUIRED)
find_package(Libdl)
if (UNIX AND NOT APPLE)
  find_package(Librt)
endif()


include_directories(
    ${FOLLY_INCLUDE_DIR}
    ${WANGLE_INCLUDE_DIR}
    ${GFLAGS_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}

    #${OPENSSL_INCLUDE_DIR}
    #${Boost_INCLUDE_DIR}
    #${LIBEVENT_INCLUDE_DIRS}
    #${DOUBLE_CONVERSION_INCLUDE_DIRS}
)

target_link_libraries(__REPLACE__ 
    __REPLACE__-lib
    ${WANGLE_LIBRARIES}
    ${FOLLY_LIBRARIES}
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${GLOG_LIBRARIES}
    ${GFLAGS_LIBRARIES}
    ${LIBEVENT_LIBRARIES}
    ${DOUBLE_CONVERSION_LIBRARIES}
    ${LIBDL_LIBRARIES}
    ${LIBRT_LIBRARIES}
)


enable_testing()
include(ExternalProject)
ExternalProject_Add(
      gtest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.8.0
      PREFIX gtest
      # Disable install step
      INSTALL_COMMAND ""
      LOG_DOWNLOAD ON
      LOG_UPDATE 1
      LOG_CONFIGURE ON
      LOG_BUILD ON
      LOG_TEST 1
      LOG_INSTALL 1
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)
add_library(libgmock_main IMPORTED STATIC GLOBAL)
add_dependencies(libgmock_main gtest)

# Set gmock properties
ExternalProject_Get_Property(gtest source_dir binary_dir)
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
set_target_properties(libgmock_main PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock_main.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

include_directories("${source_dir}/googlemock/include")
include_directories("${source_dir}/googletest/include")

add_executable(__REPLACE__-test test/unit_test.cc)
target_link_libraries(__REPLACE__-test 
    __REPLACE__-lib
    ${WANGLE_LIBRARIES}
    ${FOLLY_LIBRARIES}
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${GLOG_LIBRARIES}
    ${GFLAGS_LIBRARIES}
    ${LIBEVENT_LIBRARIES}
    ${DOUBLE_CONVERSION_LIBRARIES}
    ${LIBDL_LIBRARIES}
    ${LIBRT_LIBRARIES}
    libgmock 
    libgmock_main
)

include(GoogleTest)
gtest_add_tests(TARGET      __REPLACE__-test
                TEST_LIST   unit
)

set_tests_properties(${unit}   PROPERTIES TIMEOUT 10)
