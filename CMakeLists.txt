cmake_minimum_required(VERSION 3.22.0)    

project(udpcan CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(GENERATE_PYBIND TRUE)

set(INC_DIRS
    ./include/
    ./modules/net/include/
    ./modules/can/include/
)

set(SOURCES
    ./src/udpcan.cpp
)

set(TEST_SOURCES
    ./test/test.cpp
)

set(PY_SOURCES
    ./pybind/bindings.cpp
)

add_subdirectory(./modules/can)
add_subdirectory(./modules/net)

################## lib ##################

add_library(udpcan STATIC ${SOURCES})

set_target_properties(udpcan PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../lib/"
    LIBRARY_OUTPUT_DIRECTORY "../lib/"
    RUNTIME_OUTPUT_DIRECTORY "../lib/"
)

target_include_directories(udpcan PRIVATE
    ${INC_DIRS}
)

target_link_libraries(udpcan PRIVATE
    udpcan-can
    udpcan-net
)

target_link_directories(udpcan PRIVATE
    ./lib/
)

################## test ##################

add_executable(udpcan-test ${SOURCES} ${TEST_SOURCES})

target_include_directories(udpcan-test PRIVATE
    ${INC_DIRS}
)

target_link_libraries(udpcan-test PRIVATE
    udpcan-can
    udpcan-net
)

target_link_directories(udpcan-test PRIVATE
    ./lib/
)

if(GENERATE_PYBIND)
    set(pybind11_DIR /home/davidgmolnar/.local/lib/python3.10/site-packages/pybind11/share/cmake/pybind11) 
    find_package(pybind11 REQUIRED)

    pybind11_add_module(udpcanpy ${PY_SOURCES} ${SOURCES})

    target_include_directories(udpcanpy PUBLIC
        ${PYBIND_INC}
        ${INC_DIRS}
    )

    target_link_libraries(udpcanpy PUBLIC
        udpcan-can
        udpcan-net
        udpcan
    )

    target_link_directories(udpcanpy PUBLIC
        ./lib/
    )
endif()