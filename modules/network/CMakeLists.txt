cmake_minimum_required(VERSION 3.22.0)    

project(udpcan-net CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(INC_DIRS
    ./include/
    ../../../COM-2024-udpcan-net/include/
)

set(SOURCES
    ./src/network.cpp
)

set(TEST_SOURCES
    ./test/test.cpp
)

################## lib ##################

add_library(udpcan-network STATIC ${SOURCES})

set_target_properties(udpcan-network PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../../../lib/"
    LIBRARY_OUTPUT_DIRECTORY "../../../lib/"
    RUNTIME_OUTPUT_DIRECTORY "../../../lib/"
)

target_include_directories(udpcan-network PRIVATE
    ${INC_DIRS}
)

if (WIN32)
    target_link_libraries(udpcan-network PRIVATE
        wsock32
        udpcan-net
    )
else()
    target_link_libraries(udpcan-network PRIVATE
        udpcan-net
    )
endif()

target_link_directories(udpcan-network PRIVATE
    ../../../COM-2024-udpcan-net/lib/
)

################## test ##################

add_executable(udpcan-network-test ${SOURCES} ${TEST_SOURCES})

target_include_directories(udpcan-network-test PRIVATE
    ${INC_DIRS}
)

if (WIN32)
    target_link_libraries(udpcan-network-test PRIVATE
        wsock32
        udpcan-net
    )
else()
    target_link_libraries(udpcan-network-test PRIVATE
        udpcan-net
    )
endif()

target_link_directories(udpcan-network-test PRIVATE
    ../../../COM-2024-udpcan-net/lib/
)