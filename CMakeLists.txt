cmake_minimum_required(VERSION 3.22)
project(ws)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

# 添加模块目录
add_subdirectory(extern)

# Main project
file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Main project
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# Ensure project builds after jsoncpp
add_dependencies(${PROJECT_NAME} jsoncpp)

# Link jsoncpp_lib target
target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp_lib)
