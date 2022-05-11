cmake_minimum_required(VERSION 3.21)
project(MiniServer)
set(MINI_SERVER_VERSION_MAJOR 0)
set(MINI_SERVER_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(MiniServer main.cpp)

MESSAGE(STATUS "compiler is: " ${CMAKE_CXX_COMPILER_ID})

## Unit test
option(ENABLE_COVERAGE "Enable Test Coverage" ON)
if (ENABLE_COVERAGE)
    include(cmake/CodeCoverage.cmake)
endif (ENABLE_COVERAGE)

set(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS}")
## Unit test

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow -fPIC")

# treat warnings as errors
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -Werror")

# static link libgcc and libstdc++
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

file(MAKE_DIRECTORY build)

cmake_policy(SET CMP0015 NEW)
link_directories(build)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

## detect gcc version to decide which c std should we use
execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -v
        OUTPUT_VARIABLE COMPILER_VERSION_OUTPUT
        ERROR_VARIABLE COMPILER_VERSION_ERROR
)

set(FLAG_LIST
        -Wall
        -Wextra
        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Weffc++
        -Wformat=2
        -Wmissing-format-attribute
        -Wno-non-template-friend
        -Wno-unused-parameter
        -Woverloaded-virtual
        -Wwrite-strings
        )
foreach (FLAG ${FLAG_LIST})
    set(CMAKE_CXX_FLAGS "${TRINIDAD_CXX_FLAGS} ${FLAG}")
endforeach ()


# log
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPDLOG_NO_EXCEPTIONS=ON")
include_directories(third_party/spdlog/include)

# abseil-cpp
find_package(absl CONFIG REQUIRED)

# check library exists
function(check_lib_exist LIB_NAME)
    set(PATH_VAR_NAME "${LIB_NAME}_PATH")
    find_library(${PATH_VAR_NAME} NAMES ${LIB_NAME})
    if ("${${PATH_VAR_NAME}}" STREQUAL "${PATH_VAR_NAME}-NOTFOUND")
        ## sending warrning allows user to compile the lib without unnecessary dependency
        message(WARNING "can not find library ${LIB_NAME}")
    else ()
        message(STATUS "found library ${LIB_NAME} at ${${PATH_VAR_NAME}}")
    endif ()
endfunction(check_lib_exist)

check_lib_exist(pthread)

set(ABSL_LIBS absl::strings absl::base absl::str_format)

add_subdirectory(src)
add_subdirectory(test)

set(ALL_SRC
        src
        test)

# custom targets
add_custom_target(check
        COMMAND third_party/cpplint/cpplint.py --recursive ${ALL_SRC} 2>&1 > /var/tmp/cpplint.out)
add_custom_target(docs
        COMMAND doxygen docs/Doxyfile)
