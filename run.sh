#！/bin/bash
cd src
make clean
make
cd ../bin
./FM_Partitioner -c ../testcases/p2-1.cells -n ../testcases/p2-1.nets -o p2-1.out
./FM_Partitioner -c ../testcases/p2-2.cells -n ../testcases/p2-2.nets -o p2-2.out
./FM_Partitioner -c ../testcases/p2-3.cells -n ../testcases/p2-3.nets -o p2-3.out
./FM_Partitioner -c ../testcases/p2-4.cells -n ../testcases/p2-4.nets -o p2-4.out
