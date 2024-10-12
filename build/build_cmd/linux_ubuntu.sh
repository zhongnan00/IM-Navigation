echo ""
echo "****************************"
echo "* Building Linux Ubuntu... *"
echo "****************************"
echo ""

echo "[ Running cmake for Linux Ubuntu ]"
cmake -G Ninja -Bbuild_linux_ubuntu -DBUILD_SYSTEM_NAME=ubuntu -DBUILD_UI=OFF .

echo "[ Building...  ]"
cd build_linux_ubuntu
ninja -j 0 2>&1|tee build_linux_ubuntu_output.txt

cd ..
echo "[ Saving output...  ]"

mkdir -p build_output/build_results/x86
cp build_linux_ubuntu/build_linux_ubuntu_output.txt build_output/build_results/x86

mkdir -p build_output/app/x86

cp ./build_linux_ubuntu/app/test/hello_world build_output/app/x86/