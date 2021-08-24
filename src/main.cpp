#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "part_graph.h"

using namespace std;

char *cellFile, *netFile, *outputFile;

void printUsage(char* str)
{
	fprintf(stderr, "Usage: %s [-c cells_file] [-n nets_file] [-o output_file]\n", str);
}

// parse command
void parseCmd(int argc, char **argv)
{
    if (argc < 7) { 
        printUsage(argv[0]); 
        exit(EXIT_FAILURE);
    }
    char opt = 0;
	char output[500];
    while ((opt = getopt(argc, argv, "c:n:o:h")) != -1){
        switch (opt){
            case 'c':
				cellFile = optarg;
                break;
            case 'n':
                netFile = optarg;
                break;
            case 'o':
                outputFile = optarg;
				strcpy(output, "../output/");
				strcat(output, optarg);
				outputFile = (char*)new char[500];
				strcpy(outputFile, output);
                break;
            case 'h' : default:
                printUsage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv)
{
    parseCmd(argc, argv);
    part_graph a;
    net b;
    double io_time, computation_time;
    double iotime1 = (double)clock();
    a.getcells((char*)cellFile);
    a.init_part(); //initial partition
    a.getnets((char*)netFile);
    double hparttime = (double)clock() - iotime1;
    io_time = (double)(hparttime / CLOCKS_PER_SEC);
    a.init_gain();
    a.move();
    computation_time = (double)(((double)clock() - hparttime) / CLOCKS_PER_SEC);
    //a.print_result();

    ofstream myresult(outputFile,ios::trunc);
	if (!myresult) 
    {
        cout<<"error opening file ";
        //cin>>name;
        exit(0);
    }

	myresult << "cut_size " << a.opt_cutsize << "\n";
	myresult << "A " << a.opt_num[0] << "\n";
    for(vector<int>::iterator it = a.opt_block[0].begin(); it != a.opt_block[0].end(); ++it)
    {
        int tmp = *it;
        myresult<<"c"<<tmp<<"\n";
    }
	myresult << "B " << a.opt_num[1] << "\n";
	for(vector<int>::iterator it = a.opt_block[1].begin(); it != a.opt_block[1].end(); ++it)
    {
        int tmp = *it;
        myresult<<"c"<<tmp<<"\n";
    }
    cout<<"Input : \""<<cellFile<<"\", \""<<netFile<<"\""<<endl;
    cout<<"IO time = "<<io_time<<"s"<<endl;
    cout<<"Computation time = "<<computation_time<<"s"<<endl;
    a.print_result();
}