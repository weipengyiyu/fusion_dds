include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

if(DEFINED ENV{USE_CYCLONEDDS})
    set(USE_CYCLONEDDS $ENV{USE_CYCLONEDDS})
endif()
if(DEFINED ENV{USE_FASTDDS})
    set(USE_FASTDDS $ENV{USE_FASTDDS})
endif()
if(DEFINED ENV{USE_CONNEXTDDS})
    set(USE_CONNEXTDDS $ENV{USE_CONNEXTDDS})
endif()


if(USE_CYCLONEDDS AND USE_FASTDDS)
    message(FATAL_ERROR "not support setting \"USE_CYCLONEDDS\" and \"USE_FASTDDS\" simultaneously to avoid potential lib confiction")
endif()


# set(ROS2_PATH "/opt/ros/foxy")
set(DAS_DDS_LIB_PATH ${INSTALL_PATH}/com)


################################################################################
if(USE_FASTDDS)
    message("----- fastdds -----")
    add_definitions(-DUSE_FASTDDS)
    # include_directories(
    #   ${ROS2_PATH}/include
    # )
    # link_directories(${ROS2_PATH}/lib)
    # list(APPEND DDS_DEP_LIBS fastcdr)

    # Find requirements
    if(NOT fastcdr_FOUND)
        find_package(fastcdr REQUIRED)
    endif()

    if(NOT foonathan_memory_FOUND)
        find_package(foonathan_memory REQUIRED)
    endif()

    if(NOT fastrtps_FOUND)
        find_package(fastrtps REQUIRED)
    endif()
    list(APPEND DDS_DEP_LIBS fastcdr foonathan_memory fastrtps)
################################################################################
elseif(USE_CYCLONEDDS)
    message("----- cyclonedds -----")
    list(APPEND CMAKE_PREFIX_PATH ${iceoryx_DIR})
    add_definitions(-DUSE_CYCLONEDDS)
    find_package(CycloneDDS-CXX CONFIG REQUIRED)
    message("----CycloneDDS-CXX_CONSIDERED_CONFIGS:" ${CycloneDDS-CXX_CONSIDERED_CONFIGS})
    message("----CycloneDDS-CXX_CONSIDERED_VERSIONS:" ${CycloneDDS-CXX_CONSIDERED_VERSIONS})
    list(APPEND DDS_DEP_LIBS CycloneDDS-CXX::ddscxx)
################################################################################
elseif(USE_CONNEXTDDS)
    message("----- connextdds -----")
    add_definitions(-DUSE_CONNEXTDDS)
    # TODO
################################################################################
endif()

message("DDS_DEP_LIBS: " ${DDS_DEP_LIBS})
message("DAS_DDS_LIB_PATH: " ${DAS_DDS_LIB_PATH})