
SCRIPT_PATH=$(cd $(dirname "$0"); pwd)
INPUT_PATH=${SCRIPT_PATH}/../cmake_build/install/third_party/Fast-DDS-Gen/fastddsgen/java
OUTPUT_PATH=${SCRIPT_PATH}/../idl_src/fastdds

echo "Usage: script xxxx.idl [xxxx.idl]"
echo "Input IDL: $@"
echo

java -jar ${INPUT_PATH}/fastddsgen.jar -typeros2 -cs -d ${OUTPUT_PATH} "$@"


if [ $? -eq 0 ]; then
    echo
    echo "Files are output to [${OUTPUT_PATH}]"
    echo
fi
