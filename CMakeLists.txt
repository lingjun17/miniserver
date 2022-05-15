cmake_minimum_required(VERSION 3.19)
project(orderbook)
set(MINI_SERVER_VERSION_MAJOR 0)
set(MINI_SERVER_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

MESSAGE(STATUS "compiler is: " ${CMAKE_CXX_COMPILER_ID})

## Unit test
option(ENABLE_COVERAGE "Enable Test Coverage" OFF)
message('PROJECT_SOURCE_DIR: ' ${PROJECT_SOURCE_DIR})
if (ENABLE_COVERAGE)
    message("ENABLE_COVERAGE=ON")
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS}")
    include(./CodeCoverage.cmake)
    set(COVERAGE_EXCLUDES
            '${PROJECT_SOURCE_DIR}/third_party/*')
    setup_target_for_coverage_gcovr_html(
            NAME coverage
            EXECUTABLE test --gtest_output=xml:/var/tmp/tests/unitTestReport.xml --gtest_color=yes
            BASE_DIRECTORY "."
            EXCLUDE "${PROJECT_SOURCE_DIR}/third_party/*")
else()
    set(CMAKE_CXX_FLAGS "-g -O0 -Werror ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-g -O0 -Werror ${CMAKE_C_FLAGS}")
endif (ENABLE_COVERAGE)
## Unit test

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow -fPIC")

cmake_policy(SET CMP0114 NEW)

# output dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# include dir
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPDLOG_NO_EXCEPTIONS=ON")
include_directories(third_party/spdlog/include)
include_directories(third_party/abseil-cpp)

# link dir
link_directories(${CMAKE_BINARY_DIR}/build)
# static link libgcc and libstdc++
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# compile third party start
# enable absl
#find_package(absl CONFIG REQUIRED)

option(ABSL_PROPAGATE_CXX_STD "ABSL_PROPAGATE_CXX_STD" ON)
add_subdirectory(third_party/abseil-cpp)
set(ABSL_LIBS absl::strings absl::base absl::str_format)

# compile third party end

set(FLAG_LIST
        -Wall
        -Wextra
        -Wno-error=unused-but-set-variable
        -Wcast-align
        -Wcast-qual
        -Wconversion
        #-Weffc++
        -Wformat=2
        #-Wmissing-format-attribute
        -Wno-non-template-friend
        #-Wno-unused-parameter
        -Woverloaded-virtual
        -Wwrite-strings
        )
foreach (FLAG ${FLAG_LIST})
    set(CMAKE_SELF_CODE_CXX_FLAGS "${CMAKE_SELF_CODE_CXX_FLAGS} ${FLAG}")
endforeach ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_SELF_CODE_CXX_FLAGS}")

add_subdirectory(src)
add_subdirectory(test)

file(GLOB_RECURSE ALL_SRC "src/*.cpp" "src/*.h" "src/*.hpp" "src/*.cc")

# custom targets
add_custom_target(check COMMAND python3 ../third_party/cpplint/cpplint.py --recursive ${ALL_SRC} 2>&1 > /var/tmp/cpplint.out)
add_custom_target(docs COMMAND doxygen docs/Doxyfile)
