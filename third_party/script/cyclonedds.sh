#!/bin/bash
# 
# Copyright(C)2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
# 

SCRIPT_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)
MODULE_NAME=cyclonedds
INSTALL_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/${MODULE_NAME}
PREFIX_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/iceoryx/  #录下有include和lib等子目录，CMake将会在/include查找头文件，在/lib查找库文件
BINDING_C_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/iceoryx/lib/cmake/iceoryx_binding_c/
PLATFORM_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/iceoryx/lib/cmake/iceoryx_platform/
HOOFS_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/iceoryx/lib/cmake/iceoryx_hoofs/
POSH_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/iceoryx/lib/cmake/iceoryx_posh/
DUST_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/iceoryx/lib/cmake/iceoryx_dust/

JOB_NUM=8

# DEP_LIBS=(
#     attr.sh
#     gettext.sh
#     acl.sh
#     iceoryx.sh
# )
################################################################################
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
# if [ "X${ENABLE_SHM}" = "X1" ]; then
#     for lib in ${DEP_LIBS[@]}
#     do
#         ${SCRIPT_PATH}/${lib}
#         if [ $? -ne 0 ]; then
#             echo "build dependencies \"${lib}\" failed!"
#             exit 1
#         fi
#     done
# fi

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

mkdir -p build
pushd build && rm -rf *
if [ "X${CROSS_COMPILE_FLAG}" = "X1" ]; then
    if [ "X${ENABLE_SHM}" = "X1" ]; then
        cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DCMAKE_PREFIX_PATH=${PREFIX_PATH} -Dcyclonedds_WITH_BINDINGS=off -DCMAKE_TOOLCHAIN_FILE=${TOOL_CHAIN_FILE} \
            -Diceoryx_binding_c_DIR=${BINDING_C_PATH} -Diceoryx_platform_DIR=${PLATFORM_PATH} -Diceoryx_hoofs_DIR=${HOOFS_PATH} -Diceoryx_posh_DIR=${POSH_PATH} -Diceoryx_dust_DIR=${DUST_PATH} \
            -DENABLE_SHM=YES -DCMAKE_BUILD_TYPE=${THIRD_PARTY_CMAKE_BUILD_TYPE}
    else
        cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DCMAKE_TOOLCHAIN_FILE=${TOOL_CHAIN_FILE} -DCMAKE_PREFIX_PATH=${PREFIX_PATH} -DCMAKE_BUILD_TYPE=${THIRD_PARTY_CMAKE_BUILD_TYPE}
    fi
else
    if [ "X${ENABLE_SHM}" = "X1" ]; then
        cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DCMAKE_PREFIX_PATH=${PREFIX_PATH} -Diceoryx_platform_DIR=${PLATFORM_PATH}  \
            -Diceoryx_hoofs_DIR=${HOOFS_PATH} -Diceoryx_posh_DIR=${POSH_PATH} -Diceoryx_dust_DIR=${DUST_PATH}  \
            -DENABLE_SHM=YES -DCMAKE_BUILD_TYPE=${THIRD_PARTY_CMAKE_BUILD_TYPE}
    else
        cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DCMAKE_PREFIX_PATH=${PREFIX_PATH} -DBUILD_EXAMPLES=On
    fi
fi

make -j ${JOB_NUM}
make install
popd
rm -rf build