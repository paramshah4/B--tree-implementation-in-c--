set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR})
add_library(BPLUSTREE STATIC b_plus_tree.cpp)
target_include_directories(BPLUSTREE PUBLIC ${PROJECT_SOURCE_DIR}/include)
