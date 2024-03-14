cmake_minimum_required(VERSION 3.15)
project(com VERSION 1.0)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/dds.cmake)

# specify the C++ standard
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_11)

# add compiler warning flags just when building this project via
# the BUILD_INTERFACE genex
# set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
# set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
# target_compile_options(compiler_flags INTERFACE
#     "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
#     "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
# )

# find_package(glog CONFIG REQUIRED)

# message("glog_DIR: " ${glog_DIR})
# message("gRPC_DIR: " ${gRPC_DIR})
# message("CycloneDDS-CXX_DIR: " ${CycloneDDS-CXX_DIR})


if(USE_FASTDDS)
    list(APPEND com_src
        com_factory.cpp
        fastdds/pub.cpp
        fastdds/sub.cpp
    )
    find_package(fastcdr CONFIG REQUIRED)
    find_package(foonathan_memory CONFIG REQUIRED)
    find_package(fastrtps CONFIG REQUIRED)

    add_library(com SHARED
        ${com_src}
    )
    target_include_directories(com
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC "${PROJECT_BINARY_DIR}"
    )
    target_link_libraries(com
        # ${DDS_DEP_LIBS}
        fastcdr foonathan_memory fastrtps
        # glog::glog
    )
    install(
        TARGETS com
        DESTINATION ${INSTALL_PATH}/com
    )
elseif(USE_CYCLONEDDS)
    list(APPEND com_src
        com_factory.cpp
        cyclonedds/pub.cpp
        cyclonedds/sub.cpp
    )
    find_package(CycloneDDS-CXX CONFIG REQUIRED)
    add_library(com SHARED
        ${com_src}
    )
    target_link_libraries(com
        # ${DDS_DEP_LIBS}
        CycloneDDS-CXX::ddscxx
        # glog::glog
    )
    target_include_directories(com
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC "${PROJECT_BINARY_DIR}"
    )

    install(
        TARGETS com
        DESTINATION ${INSTALL_PATH}/com
    )
endif()

