# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Install/Esp32IDF/444/Espressif/frameworks/esp-idf-v4.4.4/components/bootloader/subproject"
  "P:/ESP32/development_board/sample_project/build/bootloader"
  "P:/ESP32/development_board/sample_project/build/bootloader-prefix"
  "P:/ESP32/development_board/sample_project/build/bootloader-prefix/tmp"
  "P:/ESP32/development_board/sample_project/build/bootloader-prefix/src/bootloader-stamp"
  "P:/ESP32/development_board/sample_project/build/bootloader-prefix/src"
  "P:/ESP32/development_board/sample_project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "P:/ESP32/development_board/sample_project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
