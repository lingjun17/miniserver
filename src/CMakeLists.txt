project(orderbook)

# find source files
file(GLOB_RECURSE SRC_LIST "*.cpp" "*.h" "*.hpp" "*.cc")

# add libraries
set(LIBRARY ${ABSL_LIBS})

add_executable(orderbook ${SRC_LIST})
target_link_libraries(
        orderbook
        ${LIBRARY})
