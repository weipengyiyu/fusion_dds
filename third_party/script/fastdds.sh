#!/bin/bash
# 
# Copyright(C)2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
# 

SCRIPT_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)
MODULE_NAME=fastdds
INSTALL_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/${MODULE_NAME}
JOB_NUM=8

DEP_LIBS=(
    tinyxml2.sh
)

################################################################################
if [ "X${USE_FASTDDS}" != "X1" ]; then
    echo "fastdds is diabled! skip."
    exit 0
fi

# digest validate
# source ${SCRIPT_PATH}/common.sh
# MODULE_TAR=${MODULE_NAME}.tar.gz
# check_digest ${SCRIPT_PATH}/../src_tar/${MODULE_TAR}
# CHECK_RET=$?
# if [ ${CHECK_RET} -eq 1 ]; then
#     echo "file \"${MODULE_TAR}\" is updated! rebuild..."
#     rm -rf ${INSTALL_PATH}  ${SCRIPT_PATH}/../src/${MODULE_NAME}
# fi

if [ -d ${INSTALL_PATH} ]; then
    echo "third party \"${MODULE_NAME}\" already exist, skip build..."
    exit 0
fi

# build dependencies
for lib in ${DEP_LIBS[@]}
do
    ${SCRIPT_PATH}/${lib}
    if [ $? -ne 0 ]; then
        echo "build dependencies \"${lib}\" failed!"
        exit 1
    fi
done

SRC_PATH=${SCRIPT_PATH}/../src
mkdir -p ${SRC_PATH}
cd ${SRC_PATH}

if [ ! -e "${MODULE_NAME}" ]; then
    TAR_FILE_NAME=${SRC_PATH}/../src_tar/${MODULE_NAME}.tar.gz
    if [ ! -e ${TAR_FILE_NAME} ]; then
        echo "No src/${MODULE_NAME} or src_tar/${MODULE_NAME}.tar.gz file exist!"
        exit 1
    fi
    tar xzf ${TAR_FILE_NAME}
fi

echo "========building ${MODULE_NAME}..."
echo "install path: ${INSTALL_PATH}"
cd ${MODULE_NAME}

# patch
# foonathan_memory_vendor-1.3.1/CMakeLists.txt.patch.20240201
PCH=CMakeLists.txt.patch.20240201
pushd foonathan_memory_vendor-1.3.1
if [ -f "${PCH}" ]; then
    if [ -z "`which patch`" ]; then
        echo "please install \"patch\" first!"
        exit 1
    fi
    patch -N < ${PCH}
fi
popd
# Fast-DDS-2.13.1/CMakeLists.txt.patch.20240201
pushd Fast-DDS-2.13.1
if [ -f "${PCH}" ]; then
    if [ -z "`which patch`" ]; then
        echo "please install \"patch\" first!"
        exit 1
    fi
    patch -N < ${PCH}
fi
popd
# Fast-DDS-2.13.1/src/cpp/utils/threading/threading_pthread.ipp.patch.20240201
pushd Fast-DDS-2.13.1/src/cpp/utils/threading
PCH=threading_pthread.ipp.patch.20240201
if [ -f "${PCH}" ]; then
    if [ -z "`which patch`" ]; then
        echo "please install \"patch\" first!"
        exit 1
    fi
    patch -N < ${PCH}
fi
popd

# Foonathan memory
SUBMOD=foonathan_memory_vendor-1.3.1
echo "========installing ${SUBMOD}..."
mkdir -p ${SUBMOD}/build
pushd ${SUBMOD}/build && rm -rf *
if [ "X${CROSS_COMPILE_FLAG}" = "X1" ]; then
    cmake .. -DCMAKE_BUILD_TYPE=${THIRD_PARTY_CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DCMAKE_TOOLCHAIN_FILE=${TOOL_CHAIN_FILE}
else
    cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DBUILD_SHARED_LIBS=ON
fi
cmake --build . --target install -j ${JOB_NUM}
echo "========${SUBMOD} installed."
popd
rm -rf ${SUBMOD}/build

# Fast CDR
SUBMOD=Fast-CDR-2.1.3
echo "========installing ${SUBMOD}..."
mkdir -p ${SUBMOD}/build
pushd ${SUBMOD}/build && rm -rf *
if [ "X${CROSS_COMPILE_FLAG}" = "X1" ]; then
    cmake .. -DCMAKE_BUILD_TYPE=${THIRD_PARTY_CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DCMAKE_TOOLCHAIN_FILE=${TOOL_CHAIN_FILE}
else
    cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH}
fi
cmake --build . --target install -j ${JOB_NUM}
echo "========${SUBMOD} installed."
popd
rm -rf ${SUBMOD}/build

# Fast-DDS
## install asio to Fast-DDS-2.13.1/thirdparty/asio

## Fast-DDS
SUBMOD=Fast-DDS-2.13.1
echo "========installing ${SUBMOD}..."
mkdir -p ${SUBMOD}/build
pushd ${SUBMOD}/build && rm -rf *
if [ "X${CROSS_COMPILE_FLAG}" = "X1" ]; then
    cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} \
        -Dfoonathan_memory_DIR=${INSTALL_PATH}/lib/foonathan_memory/cmake -Dfastcdr_DIR=${INSTALL_PATH}/lib/cmake/fastcdr \
        -DCMAKE_TOOLCHAIN_FILE=${TOOL_CHAIN_FILE} -DCMAKE_BUILD_TYPE=${THIRD_PARTY_CMAKE_BUILD_TYPE} \
        -DTinyXML2_DIR=${INSTALL_PATH}/../tinyxml2/lib/cmake/tinyxml2
else
    cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -Dfoonathan_memory_DIR=${INSTALL_PATH}/lib/foonathan_memory/cmake \
        -Dfastcdr_DIR=${INSTALL_PATH}/lib/cmake/fastcdr -DTinyXML2_DIR=${INSTALL_PATH}/../tinyxml2/lib/cmake/tinyxml2
fi
cmake --build . --target install -j ${JOB_NUM}
if [ $? -ne 0 ]; then
    echo "${SUBMOD} build failed!"
    exit 1
fi
echo "========${SUBMOD} installed."
popd
rm -rf ${SUBMOD}/build
