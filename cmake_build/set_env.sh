#!/bin/bash
# 
# Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
# 

# dds enable share memory or not
# export ENABLE_SHM=0

set_env() {

    local SCRIPT_PATH=$(cd `dirname ${BASH_SOURCE[0]}`; pwd)
    local DAS_HOME=${SCRIPT_PATH}/install

    local DEP_LIBS_ARR=(
        ${DAS_HOME}/third_party/fastdds/lib
        ${DAS_HOME}/third_party/cyclonedds/lib
        ${DAS_HOME}/third_party/cyclonedds-cxx/lib
        ${DAS_HOME}/third_party/googletest/lib
        ${DAS_HOME}/third_party/tinyxml2/lib
    )

    ################################################################################
    local LIB_PATHS=
    for lib in ${DEP_LIBS_ARR[@]}
    do
        LIB_PATHS=${lib}:${LIB_PATHS}
    done

    local CYCLONEDDS_URI_TMP=file://${SCRIPT_PATH}/cyclonedds.xml
    # if [ "X${ENABLE_SHM}" = "X1" ]; then
    #     CYCLONEDDS_URI_TMP=file://${SCRIPT_PATH}/cyclonedds_shm.xml
    # fi
    export LD_LIBRARY_PATH="${LIB_PATHS}:${LD_LIBRARY_PATH}"
    export PATH="${DAS_HOME}/third_party/gettext-0.22/bin:${PATH}"
    export CYCLONEDDS_URI=${CYCLONEDDS_URI_TMP}
}

set_env

# echo "ENABLE_SHM: ${ENABLE_SHM}"
# echo "PATH: ${PATH}"
# echo "LD_LIBRARY_PATH: ${LD_LIBRARY_PATH}"
# echo "CYCLONEDDS_URI: ${CYCLONEDDS_URI}"
