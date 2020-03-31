mkdir build && cd build
cmake ../ -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=../gcc-toolchain.cmake
cmake --build .
cd ..
