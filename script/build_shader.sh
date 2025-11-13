#!/bin/bash

INPUT_DIR=$(pwd)/../source/display/asset/shader/es310
MSL_OUTPUT_DIR=$(pwd)/../source/display/asset/shader/metal
ES200_OUTPUT_DIR=$(pwd)/../source/display/asset/shader/es200
ES300_OUTPUT_DIR=$(pwd)/../source/display/asset/shader/es300
VK_OUTPUT_DIR=$(pwd)/../source/display/asset/shader/vk

echo "inpput path : ${INPUT_DIR}"
echo "metal output path : ${MSL_OUTPUT_DIR}"
echo "es200 output path : ${ES200_OUTPUT_DIR}"
echo "es300 output path : ${ES300_OUTPUT_DIR}"
echo "vk output path : ${VK_OUTPUT_DIR}"

python3 shader_build.py -msl ${INPUT_DIR} ${MSL_OUTPUT_DIR}
python3 shader_build.py -es300 ${INPUT_DIR} ${ES300_OUTPUT_DIR}
python3 shader_build.py -es200 ${INPUT_DIR} ${ES200_OUTPUT_DIR}
# python3 shader_build.py -vk ${INPUT_DIR} ${VK_OUTPUT_DIR}