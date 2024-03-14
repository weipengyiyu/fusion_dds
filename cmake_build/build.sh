#!/bin/bash
# 
# Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
# 
# 需要编译的目录添加到"BUILD_ARR"


SCRIPT_PATH=$(cd `dirname ${BASH_SOURCE[0]}`; pwd)

# 首次所有模块都编译后，后续调试时，可以注释掉不感兴趣的模块（e.g.只保留plugins模块），以加快编译构建
BUILD_ARR=(
    com
)

# cmake_build_type
# Debug：用于开发和调试目的的构建类型，通常包含调试符号，并禁用优化。
# Release：用于发布最终产品的构建类型，启用优化，并且不包含调试符号。
# RelWithDebInfo：启用优化，并包含调试符号，用于发布版本但仍需进行调试的情况。
# MinSizeRel：优化代码大小，用于资源受限的环境。
export THIRD_PARTY_CMAKE_BUILD_TYPE=Release


export USE_CYCLONEDDS=0
export USE_FASTDDS=1
export USE_CONNEXTDDS=0

TOOL_CHAIN_FILE=${TOOLCHAINDIR}/aarch64-toolchain.cmake
JOB_NUM=4

################################################################################
CROSS_COMPILE_FLAG=0    # 如果硬件环境没有这个变量，就手动置一
echo "=======building DAS..."
if [ -n "`env | grep CROSS_COMPILE`" ]; then
    CROSS_COMPILE_FLAG=1
    echo "!!!!in cross compile env."
    echo "TOOL_CHAIN_FILE: ${TOOL_CHAIN_FILE}"
    export TOOL_CHAIN_FILE
fi

echo "CROSS_COMPILE_FLAG: ${CROSS_COMPILE_FLAG}"
echo "USE_CYCLONEDDS: ${USE_CYCLONEDDS}"
echo "USE_FASTDDS: ${USE_FASTDDS}"

export CROSS_COMPILE_FLAG
source ${SCRIPT_PATH}/set_env.sh

# 构建第三库
echo "========building third party..."
${SCRIPT_PATH}/../third_party/script/build_all.sh
if [ $? -ne 0 ]; then
    echo "third party build all failed!"
    exit 1
fi
# exit 0

mkdir -p ${SCRIPT_PATH}/build
pushd ${SCRIPT_PATH}/build && rm -rf *

# 构建DAS模块
for dir_name in ${BUILD_ARR[@]}
do
    echo "========building ${dir_name}..."
    mkdir ${dir_name}
    pushd ${dir_name}
    mkdir -p ${SCRIPT_PATH}/install/${dir_name} && rm -rf ${SCRIPT_PATH}/install/${dir_name}/*
    if [ ${CROSS_COMPILE_FLAG} -eq 1 ]; then
        cmake -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAINDIR}/aarch64-toolchain.cmake" -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -O2" ${SCRIPT_PATH}/../${dir_name}
    else
        # cmake ${SCRIPT_PATH}/../${dir_name}
        cmake -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -O2" ${SCRIPT_PATH}/../${dir_name}
    fi
    make -j ${JOB_NUM}
    make install
    if [ $? -ne 0 ]; then
        exit 1
    fi
    popd
done
popd # ${SCRIPT_PATH}/build
rm -rf ${SCRIPT_PATH}/build

# cp -arf ${SCRIPT_PATH}/set_env.sh                            ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/release_note                          ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/cyclonedds.xml                        ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/cyclonedds_shm.xml                    ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/start_das_service.sh                  ${SCRIPT_PATH}/install/framework
# cp -arf ${SCRIPT_PATH}/run_service.sh                        ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/install/client/start_config.yaml      ${SCRIPT_PATH}/install/framework/default_start_config.yaml
# cp -arf ${SCRIPT_PATH}/das_cron.txt                          ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/clean_log.sh                          ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/query_cron.sh                         ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/run_client.sh                         ${SCRIPT_PATH}/install
# cp -arf ${SCRIPT_PATH}/das-vsomeip.json                      ${SCRIPT_PATH}/install

# find ${SCRIPT_PATH}/install/ -name "*.sh" | xargs chmod +x

# # create and upload das rpm
# if [ ${CROSS_COMPILE_FLAG} -eq 1 ]; then
#     echo "========create and upload das rpm..."
#     mkdir -p ${SCRIPT_PATH}/tmp
#     mkdir -p ${SCRIPT_PATH}/rpm_build/opt/banma/das
#     cd ${SCRIPT_PATH}/rpm_build/opt/banma/das
#     cp -arf ${SCRIPT_PATH}/install/* .
#     sed -i "s#^DAS_HOME=.*#DAS_HOME=/opt/banma/das#g" set_env.sh

#     # third party trim
#     rm -rf third_party/googletest
#     if [ ${USE_FASTDDS} -eq 0 ]; then
#         rm -rf third_party/fastdds
#     fi
#     if [ ${USE_CYCLONEDDS} -eq 0 ]; then
#         rm -rf third_party/cyclonedds*
#     fi
#     pushd third_party
#     THIRD_PARTY_ARR=(`ls | xargs`)
#     for dir in ${THIRD_PARTY_ARR[@]}
#     do
#         pushd ${dir}
#         ls | grep -v bin | grep -v lib | grep -v etc | xargs rm -rf
#         popd
#     done
#     popd
#     find third_party -name "*.pc" | xargs rm -rf
#     find third_party -name "*.cmake" | xargs rm -rf

#     tar czvf ${SCRIPT_PATH}/tmp/das.tar.gz -C ${SCRIPT_PATH}/rpm_build .
#     rm -rf ${SCRIPT_PATH}/rpm_build/opt/banma/das
#     cd ${SCRIPT_PATH}
#     ados_create_and_upload_rpm ./tmp/das.tar.gz ./das.spec
# else
#     sed -i "s#DAS_HOME=.*#DAS_HOME=\$\{SCRIPT_PATH\}#g" ${SCRIPT_PATH}/install/set_env.sh
#     sed -i "s#log_dir:.*#log_dir: ${SCRIPT_PATH}/log#g" ${SCRIPT_PATH}/install/framework/config.yaml
#     sed -i "s#/opt/banma/das#${SCRIPT_PATH}/install#g" ${SCRIPT_PATH}/install/client/start_config.yaml
#     sed -i "s#/opt/banma/das#${SCRIPT_PATH}/install#g" ${SCRIPT_PATH}/install/framework/default_start_config.yaml
#     sed -i "s#/opt/banma/das#${SCRIPT_PATH}/install#g" ${SCRIPT_PATH}/install/das_cron.txt
#     sed -i "s#/opt/banma/das#${SCRIPT_PATH}/install#g" ${SCRIPT_PATH}/install/query_cron.sh
#     sed -i "s#^DAS_LOG_PATH=.*#DAS_LOG_PATH=${SCRIPT_PATH}/log#g" ${SCRIPT_PATH}/install/query_cron.sh
#     sed -i "s#^DAS_LOG_PATH=.*#DAS_LOG_PATH=${SCRIPT_PATH}/log#g" ${SCRIPT_PATH}/install/clean_log.sh
# fi
