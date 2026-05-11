cmake_minimum_required(VERSION 3.10)
project(h248parse C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/h248_parser.c
    src/h248_transaction.c
    src/h248_commands.c
    src/h248_packages.c
    src/h248_stack.c
)

# Create library
add_library(h248 STATIC ${SOURCES})

# Example gateway
add_executable(gateway_example examples/gateway_example.c)
target_link_libraries(gateway_example h248)

# Example controller
add_executable(controller_example examples/controller_example.c)
target_link_libraries(controller_example h248)

# Tests
enable_testing()
add_subdirectory(tests)
