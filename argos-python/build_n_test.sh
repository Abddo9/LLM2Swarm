#source ~/.bashrc

rm -rf build
mkdir build
cd build
cmake ..
make -j4
cd ../../DirectIntegration/
./starter -s

cd ../argos-python