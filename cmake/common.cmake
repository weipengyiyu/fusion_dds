set(INSTALL_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake_build/install)

message("CMAKE_CROSSCOMPILING: " ${CMAKE_CROSSCOMPILING})
if(CMAKE_CROSSCOMPILING)
    add_definitions(-DNDEBUG)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# for find_package()
set(foonathan_memory_DIR ${INSTALL_PATH}/third_party/fastdds/lib/foonathan_memory/cmake)
set(fastcdr_DIR ${INSTALL_PATH}/third_party/fastdds/lib/cmake/fastcdr)
set(fastrtps_DIR ${INSTALL_PATH}/third_party/fastdds/share/fastrtps/cmake) 
set(CycloneDDS_DIR ${INSTALL_PATH}/third_party/cyclonedds/lib/cmake/CycloneDDS)
set(CycloneDDS-CXX_DIR ${INSTALL_PATH}/third_party/cyclonedds-cxx/lib/cmake/CycloneDDS-CXX)
set(GTest_DIR ${INSTALL_PATH}/third_party/googletest/lib/cmake/GTest)
set(utf8_range_DIR ${INSTALL_PATH}/third_party/protobuf/lib/cmake/utf8_range)
set(TinyXML2_DIR ${INSTALL_PATH}/third_party/tinyxml2/lib/cmake/tinyxml2)
