cmake_minimum_required(VERSION 3.27)
project(BreezeBlocks C)

set(CMAKE_C_STANDARD 23)

add_executable(BreezeBlocks main.c
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

# Find SDL2, SDL2_image and SDL2_gfx libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_gfx REQUIRED)

# Link SDL2::Main, SDL2::Image and SDL2::GFX to our project
target_link_libraries(${PROJECT_NAME} m SDL2::Main SDL2::GFX)
