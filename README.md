## How to compile
Enter the following commands in the "src" directory:
```
$ make
```
The executable file "FM_Partitioner" will be generated in the "bin/" directory
If you need to remove the file, please enter the following command:
```
$ make clean
```

## How to execute
Use the following commands in the "bin" directory
```
$ ./<exe> -c <cells> -n <nets> -o <output file name>
```
For example:
```
./FM_Partitioner -c ../testcases/p2-1.cells -n ../testcases/p2-1.nets -o p2-1.out
```
The execution result will be output to the "output" directory

## Automatic compilation and execution
Use the following commands in this directory
```
$ ./run.sh
```
will automatically compile and read the p2-1~p2-4 (.cells / .nets) test execution program in the "testcases/" directory
The execution result will be output to the "output" directory
