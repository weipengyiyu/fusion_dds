#!/bin/bash
# 
# Copyright(C)2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
# 
# 使用git_clone.sh下载相应第三方源码，或从../src_tar/xxxx.tar.gz构建

SCRIPT_PATH=$(cd `dirname $BASH_SOURCE[0]`; pwd)

THIRD_PARTY_INSTALL_PATH=${SCRIPT_PATH}/../../cmake_build/install/third_party


# 优先使用编译好的库
PKG_AARCH64_ARR=(
    # xxxx.tar.gz
)

BUILD_SCRIPT_ARR=(
    fastdds.sh
    cyclonedds-cxx.sh
    fastdds-gen.sh
)
################################################################################
echo "third party build all..."
echo "ENABLE_SHM: ${ENABLE_SHM}"

# if [ ! -e ${SCRIPT_PATH}/../src_tar/md5_local.txt ]; then
#     echo "rebuild all third parties..."
#     rm -rf ${THIRD_PARTY_INSTALL_PATH}/*
#     rm -rf ${SCRIPT_PATH}/../src/*
# fi

# Consolidate compiler generated dependencies of target protoc
# [ 80%] Built target protoc
# make[5]: *** No rule to make target `_gRPC_CPP_PLUGIN-NOTFOUND', needed by `gens/src/proto/grpc/reflection/v1alpha/reflection.grpc.pb.cc'.  Stop.
# make[4]: *** [CMakeFiles/grpc++_reflection.dir/all] Error 2
if [ "X${CROSS_COMPILE_FLAG}" = "X1" ]; then
    mkdir -p ${THIRD_PARTY_INSTALL_PATH}
    cd ${THIRD_PARTY_INSTALL_PATH}
    for pkg in ${PKG_AARCH64_ARR[@]}
    do
        if [ -d ${pkg%.tar.gz*} ]; then
            echo "third party \"${pkg%.tar.gz*}\" already exist, skip untar..."
            continue
        fi
        if [ -e ${SCRIPT_PATH}/../pkg_aarch64/${pkg} ]; then
            echo "untar \"${pkg}\"..."
            tar xzf ${SCRIPT_PATH}/../pkg_aarch64/${pkg}
        fi
    done
fi


for item in ${BUILD_SCRIPT_ARR[@]}
do
    ${SCRIPT_PATH}/${item}
    if [ $? -ne 0 ]; then
        exit 1
    fi
done

# generate md5_local.txt
pushd ${SCRIPT_PATH}/../src_tar
ls *.tar.gz | xargs md5sum > md5_local.txt
sed -i "s/  /@/g" md5_local.txt
popd
