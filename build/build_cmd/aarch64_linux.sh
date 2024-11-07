#!/bin/bash

echo "****************************"
echo "* Building Linux Ubuntu... *"
echo "****************************"

echo "[ Running cmake for Linux Ubuntu ]"
cmake -G Ninja -Bbuild_aarch64_linux -DCMAKE_TOOLCHAIN_FILE=build/cmake/aarch64_linux.cmake -DBUILD_UI=OFF -Wno-dev .

echo "[ Building...  ]"
cd build_aarch64_linux
ninja -j 0 2>&1|tee build_aarch64_linux_output.txt

cd ..
echo "[ Saving output...  ]"

mkdir -p build_output/build_results/aarch64
cp build_aarch64_linux/build_aarch64_linux_output.txt build_output/build_results/aarch64

mkdir -p build_output/app/aarch64

# cp build_aarch64_linux/app/test/hello_world build_output/app/aarch64