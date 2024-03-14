#!/bin/bash
# 
# Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
# 

cd ../ 
WORK_PATH=$(pwd)
INPUT_PATH=${WORK_PATH}/idl/
OUTPUT_PATH=${WORK_PATH}/idl_src/
CONVERT=cxx

if [ $# == 0 ] ; 
then
echo "USAGE: defualt convert cxx. "
CONVERT=cxx
elif [ $# == 1 ]
then
echo "USAGE: convert c. "
CONVERT=$1
elif [ $# != 1 ]
then
echo "USAGE: $0 from to"
echo " e.g.: ./idl_conv_cpp.sh. 
           : ./idl_conv_cpp.sh  c or cxx. "
exit 1;
fi

stridl=${INPUT_PATH:0-4}
str2=".idl"

#install/tool/test/ cd ../../ #tool/idl_cpp
# cd ../../../../ 
WORK_PATH=$(pwd)
CYCLONEDDS_LIB=${WORK_PATH}/cmake_build/install/third_party/cyclonedds/lib
CYCLONEDDS_BIN=${WORK_PATH}/cmake_build/install/third_party/cyclonedds/bin
export LD_LIBRARY_PATH=${CYCLONEDDS_LIB}/src:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${CYCLONEDDS_BIN}/src:$LD_LIBRARY_PATH

folder="${CYCLONEDDS_LIB}"
if [ ! -d "$folder" ]; then
  echo "cyclonedds/lib folder is not exist" ${CYCLONEDDS_LIB}
  exit 8
fi

#get *.idl file
check_suffix()
{
    file=$1
    echo ${file}

    if [ "${file##*.}"x = "idl"x ]; then
        filename=$(basename "$file")
        cd ${OUTPUT_PATH}
        # dir="$(pwd)/"${filename%.*}""
        dir="cyclonedds"
        if [ ! -d "$dir" ];then
        mkdir $dir
        echo "The file is created successfully"
        else
        echo "The file already exists"
        fi
        cd ${dir}
        echo $(pwd)
        ${CYCLONEDDS_BIN}/idlc -l $CONVERT $1
    fi
}    

if [ ! -d "${OUTPUT_PATH}" ]; then
echo ${OUTPUT_PATH}"****outfolder is not exist"
exit 8
fi

if [ $stridl = $str2 ]
then
    cd ${OUTPUT_PATH}
    echo "one idl"
    ${CYCLONEDDS_BIN}/idlc -l $CONVERT ${INPUT_PATH}
else
    if [ ! -d "${INPUT_PATH}" ]; then
    echo "infolder is not exist"
    exit 8
    fi
    filepath=${INPUT_PATH}
    echo "more idl"
    for file in `ls -a $filepath`
    do
        if [ -d ${filepath}/$file ]
        then
            if [[ $file != '.' && $file != '..' ]]
            then
                traverse_dir ${filepath}/$file
            fi
        else
            check_suffix ${filepath}$file
        fi
    done
fi
