cmake_minimum_required(VERSION 3.10)
project(MP3PartyPlayer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra)
else()
    add_compile_options(-O3 -DNDEBUG)
endif()

# Находим зависимости
find_package(PkgConfig REQUIRED)

pkg_check_modules(NCURSES REQUIRED ncurses)
pkg_check_modules(NOTCURSES REQUIRED notcurses)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVUTIL REQUIRED libavutil)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(PULSE REQUIRED libpulse)

# nlohmann/json
find_path(NLOHMANN_INCLUDE_DIR 
    NAMES nlohmann/json.hpp
    PATHS /usr/include /usr/local/include
)

if(NOT NLOHMANN_INCLUDE_DIR)
    message(FATAL_ERROR "nlohmann/json.hpp not found. Install nlohmann-json")
endif()

# Audiere (опционально, может отсутствовать)
find_library(AUDIERE_LIB 
    NAMES audiere
    PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if(NOT AUDIERE_LIB)
    message(WARNING "Audiere not found. Install from AUR: yay -S audiere")
endif()

find_package(Threads REQUIRED)

# Исходники
set(SOURCES
    src/Main.cpp
    src/ApiManager.cpp
    src/AudioEngine.cpp
    src/center.cpp
    src/DataManager.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# ВАЖНО: Добавляем текущую директорию для include "../headers/*"
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}  # Чтобы работало #include "../headers/..."
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${NCURSES_INCLUDE_DIRS}
    ${NOTCURSES_INCLUDE_DIRS}
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVCODEC_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${PULSE_INCLUDE_DIRS}
    ${NLOHMANN_INCLUDE_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${NCURSES_CFLAGS_OTHER}
    ${NOTCURSES_CFLAGS_OTHER}
    ${AVFORMAT_CFLAGS_OTHER}
    ${AVCODEC_CFLAGS_OTHER}
    ${AVUTIL_CFLAGS_OTHER}
    ${CURL_CFLAGS_OTHER}
    ${PULSE_CFLAGS_OTHER}
)

target_link_libraries(${PROJECT_NAME}
    ${NCURSES_LIBRARIES}
    ${NOTCURSES_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${CURL_LIBRARIES}
    ${PULSE_LIBRARIES}
    Threads::Threads
    dl
)

if(AUDIERE_LIB)
    target_link_libraries(${PROJECT_NAME} ${AUDIERE_LIB})
endif()

message(STATUS "=== Build Configuration ===")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
