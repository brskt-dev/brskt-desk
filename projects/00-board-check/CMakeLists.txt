# 00-board-check — firmware de validação da Waveshare ESP32-S3-Touch-LCD-7.
# Projeto ESP-IDF independente: compile e grave a partir desta pasta.
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# Igual ao exemplo oficial (09_lvgl_v9_demo).
add_compile_options("-Wno-attributes")
project(board_check)
