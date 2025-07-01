cmake_minimum_required(VERSION 3.10)
project(App)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(OpenCV 4 REQUIRED core imgproc videoio objdetect)

add_subdirectory(ssd1306)
add_subdirectory(utils)

add_executable(app main.cpp)

target_link_libraries(app ${OpenCV_LIBS} ssd1306 utils)