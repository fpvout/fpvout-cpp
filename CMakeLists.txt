cmake_minimum_required(VERSION 3.1.0)
project(fpvLiberator)

include(FindPkgConfig)
pkg_check_modules(libusb REQUIRED IMPORTED_TARGET libusb-1.0)

include(CTest)
enable_testing()

set (CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "src/*.cpp" )
file(GLOB HEADERS "src/*.h" )

add_executable(${PROJECT_NAME}  ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PkgConfig::libusb)
