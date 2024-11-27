cmake_minimum_required(VERSION 3.29.0)    

project(udpcan-can CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(INC_DIRS
    ./include/
)

set(SOURCES
    ./src/bitarray.cpp
)

set(TEST_SOURCES
    ./test/test.cpp
)

################## lib ##################

add_library(udpcan-can STATIC ${SOURCES})

set_target_properties(udpcan-can PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../../../lib/"
    LIBRARY_OUTPUT_DIRECTORY "../../../lib/"
    RUNTIME_OUTPUT_DIRECTORY "../../../lib/"
)

target_include_directories(udpcan-can PRIVATE
    ${INC_DIRS}
)

target_link_libraries(udpcan-can PRIVATE

)

target_link_directories(udpcan-can PRIVATE

)

################## test ##################

add_executable(udpcan-can-test ${SOURCES} ${TEST_SOURCES})

target_include_directories(udpcan-can-test PRIVATE
    ${INC_DIRS}
)

target_link_libraries(udpcan-can-test PRIVATE
    
)

target_link_directories(udpcan-can-test PRIVATE

)