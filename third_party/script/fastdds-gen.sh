#!/bin/bash
# 
# Copyright(C)2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
# 

SCRIPT_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)
MODULE_NAME=Fast-DDS-Gen
INSTALL_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party/${MODULE_NAME}


################################################################################
MDL_VER_NAME=${MODULE_NAME}

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

# sudo apt install openjdk-11-jdk
./gradlew assemble
## 进入Fast-DDS-Gen/share/fastddsgen/java
#java -jar fastddsgen.jar -cs PointCloud2.idl

cp -r ./share ${INSTALL_PATH}
