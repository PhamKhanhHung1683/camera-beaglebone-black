add_library(ssd1306 STATIC ssd1306.c)

target_include_directories(ssd1306 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})