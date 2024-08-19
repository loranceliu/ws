# 设置Boost库的路径
set(BOOST_ROOT  /usr/include/boost)

# 查找Boost库的组件
find_package(Boost REQUIRED COMPONENTS system)

message("Boost version: ${Boost_VERSION}")
message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message("Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message("Boost_LIBRARIES: ${Boost_LIBRARIES}")

# 添加Boost库的头文件路径
include_directories(${Boost_INCLUDE_DIRS})

# 添加Boost库的链接路径
link_directories(${Boost_LIBRARY_DIRS})

add_library(boost_lib INTERFACE)

# 添加Boost库的链接选项
target_link_libraries(boost_lib INTERFACE ${Boost_LIBRARIES})