#ifndef PART_GRAPH_HPP
#define PART_GRAPH_HPP

#ifndef NULL
#define NULL 0
#endif

#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<vector>
#include<list>
using namespace std;

#define another(num) ((num)?0:1)

class node {
public:
    int data;
    node *next, *prev;
    node(int);
    node();
};

class cell {
public:
    int number, size;
    int gain;
    int block;
    bool locked;
    vector<int>::iterator ptr;
    vector<int> nlist;
    cell(); //constructor
    cell(int, int);
};

class net {
public:
    int number;
    int count_block[2];
    int unlock[2];  // # of unlocked cells in each block
    int lock[2];    // # of locked cells in each block
    vector<int> clist;
    net(){}//constructor
};

class part_graph {
public:
    int totalcellsize;
    int max_cell_num;
    int numcells; //total number of cell 
    int numnets; //total number of net
    int maxgain[2]; //maximum gain in each block
    int pmax, pmin;
    int pass;
    int opt_pass;
    int opt_cutsize;
    int max_gain_sum; 
    int init_cutsize;
    int cutsize;
    int area[2]; //area of each side
    int area_tmp1[2]; //temp area value of each side while moving
    int area_tmp2[2]; //temp area value of each side while moving
    int opt_num[2];
    vector<int> *bucket[2];
    vector<int> opt_block[2];
    //vector<int> *bucket1,*bucket2;
    list<int> free;
    cell *cells;
    net *nets;

    part_graph(){} //constructor

    void getcells(char *);
    void getnets(char *);
    bool balance(int, int);
    void init_part();
    void init_gain(); //compute initial gain and store it into the bucketlist
    void move(); //peform moving
    int compute_cutsize();
    void update_gain(int);
    void printcells();
    void printnets();
    void print_result();
};

#endif