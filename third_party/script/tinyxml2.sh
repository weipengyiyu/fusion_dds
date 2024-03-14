#!/bin/bash
# 
# Copyright(C)2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
# 

SCRIPT_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)
MODULE_NAME=tinyxml2
INSTALL_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/${MODULE_NAME}
VER=10.0.0
JOB_NUM=8

################################################################################
MDL_VER_NAME=${MODULE_NAME}-${VER}
# digest validate
# source ${SCRIPT_PATH}/common.sh
# MODULE_TAR=${MDL_VER_NAME}.tar.gz
# check_digest ${SCRIPT_PATH}/../src_tar/${MODULE_TAR}
# CHECK_RET=$?
# if [ ${CHECK_RET} -eq 1 ]; then
#     echo "file \"${MODULE_TAR}\" is updated! rebuild..."
#     rm -rf ${INSTALL_PATH}  ${SCRIPT_PATH}/../src/${MDL_VER_NAME}
# fi

if [ -d ${INSTALL_PATH} ]; then
    echo "third party \"${MODULE_NAME}\" already exist, skip build..."
    exit 0
fi

SRC_PATH=${SCRIPT_PATH}/../src
mkdir -p ${SRC_PATH}
cd ${SRC_PATH}

if [ ! -e "${MDL_VER_NAME}" ]; then
    TAR_FILE_NAME=${SRC_PATH}/../src_tar/${MDL_VER_NAME}.tar.gz
    if [ ! -e ${TAR_FILE_NAME} ]; then
        echo "No src/${MDL_VER_NAME} or src_tar/${MDL_VER_NAME}.tar.gz file exist!"
        exit 1
    fi
    tar xzf ${TAR_FILE_NAME}
fi

echo "========building ${MDL_VER_NAME}..."
echo "install path: ${INSTALL_PATH}"
cd ${MDL_VER_NAME}

mkdir -p build
pushd build && rm -rf *
if [ "X${CROSS_COMPILE_FLAG}" = "X1" ]; then
    cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DCMAKE_TOOLCHAIN_FILE=${TOOL_CHAIN_FILE} -DCMAKE_BUILD_TYPE=${THIRD_PARTY_CMAKE_BUILD_TYPE}
else
    cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH}
fi
make -j ${JOB_NUM}
make install
popd
rm -rf build