project(test)

# Sources
file(GLOB_RECURSE SRC_LIST "*.cpp" "*.h" "*.hpp" "*.cc")

set(LIBRARY ${ABSL_LIBS})

# unit-test on googletest
include_directories(BEFORE ../third_party/gtest/googletest)
include_directories(BEFORE ../third_party/gtest/googletest/include)
# add gmock
include_directories(BEFORE ../third_party/gtest/googlemock)
include_directories(BEFORE ../third_party/gtest/googlemock/include)

# Executable
set(SRC_LIST
        ${SRC_LIST}
        ../src/utils/BigDecimal.cpp
        ../src/model/Order.cpp
        ../src/model/OrderBook.cpp
        ../src/executor/OrderProcessor.cpp
        ../src/storage/EventStore.cpp
        ../src/model/MatchEvent.cpp)
add_executable(TestRunner
        ${SRC_LIST}
        ../third_party/gtest/googlemock/src/gmock-all.cc
        ../third_party/gtest/googletest/src/gtest-all.cc)
target_link_libraries(TestRunner ${LIBRARY} )
