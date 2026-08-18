cmake_minimum_required(VERSION 3.20)

set(TARGET game)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("./vendor/glad")
add_subdirectory("./vendor/glfw")
add_subdirectory("./vendor/glm")
add_subdirectory("./vendor/spdlog")
add_subdirectory("./vendor/enkiTS")

project(
    ${TARGET}
    LANGUAGES CXX C
    DESCRIPTION "a mining tycoon"
    VERSION 0.0.1
)

file(GLOB_RECURSE target_src "./source/*.cc")

add_executable(${TARGET})
target_sources(
    ${TARGET}
    PRIVATE 
    ${target_src}
)
target_include_directories(
    ${TARGET}
    PRIVATE
    "./include/"
)
target_link_libraries(
    ${TARGET}
    PRIVATE
    glad
    glfw
    glm
    spdlog
    enkiTS
)
target_compile_definitions(
    ${TARGET}
    PRIVATE 
    GLFW_INCLUDE_NONE
    GLM_ENABLE_EXPERIMENTAL
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_compile_options(${TARGET} PRIVATE -ggdb -Wall -fno-omit-frame-pointer)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "DebugSan")
target_compile_options(${TARGET} PRIVATE -fsanitize=address,undefined,leak -ggdb -Wall -fno-omit-frame-pointer)
target_link_options(${TARGET} PRIVATE -fsanitize=address,undefined,leak)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${TARGET} PRIVATE -03)
endif()
