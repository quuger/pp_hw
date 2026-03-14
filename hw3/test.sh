mkdir build
cd build
cmake ..
make
./main >out
python3 ../checker.py <out
echo TEST PASSED
