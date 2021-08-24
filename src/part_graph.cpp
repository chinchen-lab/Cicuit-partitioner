#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <list>
#include <time.h>
#include "part_graph.h"

using namespace std;

node::node(int num) { data = num; }
node::node() { data = -1; }
cell::cell() { number = 0; }
cell::cell(int n, int s)
{
    number = n;
    size = s;
}

void part_graph::getcells(char *filename)
{
    int numtmp, sizetmp;
    max_cell_num = totalcellsize = 0;
    FILE *pFile;
    pFile = fopen(filename, "r");
    numcells = 0;

    while (!feof(pFile))
    {
        fscanf(pFile, "%*c%d %*d\n", &numtmp);
        if (numtmp > max_cell_num)
            max_cell_num = numtmp;
        numcells++;
    }

    cells = new cell[max_cell_num + 1];
    rewind(pFile);

    while (!feof(pFile))
    {
        fscanf(pFile, "%*c%d %d\n", &numtmp, &sizetmp);
        cells[numtmp].number = numtmp;
        cells[numtmp].size = sizetmp;
        cells[numtmp].locked = false;
        totalcellsize += sizetmp;
    }
    fclose(pFile);
}

void part_graph::getnets(char *filename)
{
    int numtmp, ct1, ct2;
    FILE *pFile;
    char ch;
    int i = -3;
    init_cutsize = 0;

    pFile = fopen(filename, "r"); //read net file
    if (NULL == pFile)
    {
        cout << "Open failure:" << filename << endl;
        exit(1);
    }

    fseek(pFile, i, SEEK_END);
    ch = fgetc(pFile);
    while (ch != 'T') //search from the end of file to get the maximum nuber of net
    {
        fseek(pFile, --i, SEEK_END);
        ch = fgetc(pFile);
    }
    ch = fgetc(pFile);
    fscanf(pFile, "n%d {", &numtmp);
    fseek(pFile, i, SEEK_END);
    numnets = numtmp;
    nets = new net[numnets + 1];
    rewind(pFile);
    while (!feof(pFile))
    {
        ch = fgetc(pFile);
        if (ch == 'N')
        {
            fseek(pFile, 4, SEEK_CUR);
            fscanf(pFile, "%d { c%d c%d ", &numtmp, &ct1, &ct2);
            nets[numtmp].number = numtmp;
            nets[numtmp].count_block[0] = nets[numtmp].count_block[1] = 0;
            nets[numtmp].lock[0] = 0;
            nets[numtmp].lock[1] = 0;
            nets[numtmp].unlock[0] = 0;
            nets[numtmp].unlock[1] = 0;

            cells[ct1].nlist.push_back(numtmp);
            cells[ct2].nlist.push_back(numtmp);

            nets[numtmp].clist.push_back(ct1);
            nets[numtmp].clist.push_back(ct2);

            nets[numtmp].count_block[cells[ct1].block]++;
            nets[numtmp].count_block[cells[ct2].block]++;
            nets[numtmp].unlock[cells[ct1].block]++;
            nets[numtmp].unlock[cells[ct2].block]++;

            ch = fgetc(pFile);
            while (ch != '}')
            {
                if (ch == '\n')
                {
                    fscanf(pFile, " c%d", &ct1);
                    cells[ct1].nlist.push_back(numtmp);
                    nets[numtmp].clist.push_back(ct1);
                    nets[numtmp].count_block[cells[ct1].block]++;
                    nets[numtmp].unlock[cells[ct1].block]++;
                }
                else if (ch == 'c')
                {
                    fscanf(pFile, "%d", &ct1);
                    cells[ct1].nlist.push_back(numtmp);
                    nets[numtmp].clist.push_back(ct1);
                    nets[numtmp].count_block[cells[ct1].block]++;
                    nets[numtmp].unlock[cells[ct1].block]++;
                }
                ch = fgetc(pFile);
            }
        }
    }

    fclose(pFile);

    pmax = 0;
    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].number != 0)
        {
            int net_array_size = cells[i].nlist.size();
            if (net_array_size > pmax)
                pmax = net_array_size;
        }
    }
    pmin = -pmax;

    init_cutsize = compute_cutsize();
}

bool part_graph::balance(int a, int b)
{
    int n = totalcellsize;
    if (abs(a - b) < n / 10)
        return true;
    else
        return false;
}

void part_graph::init_part()
{
    area[0] = 0, area[1] = 0;
    srand(time(NULL));
    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].number != 0)
        {
            int side = rand() % 2;
            cells[i].block = side;
            area[side] += cells[i].size;

            if (!balance(area[0], area[1]))
            {
                area[side] -= cells[i].size;
                side = another(side);
                cells[i].block = side;
                area[side] += cells[i].size;
            }
        }
    }
}

void part_graph::init_gain() //compute initial gain and store it into the bucketlist
{
    //Initialize
    for (int i = 1; i <= max_cell_num; i++)
        cells[i].gain = 0;

    int from, to, tmp;
    int n, g;

    /*for(int i = 1; i <= max_cell_num; i++)
    {
        if(cells[i].number != 0 && !cells[i].locked)
        {
            from = cells[i].block;
            to = another(from);
            for(vector<int>::iterator it = cells[i].nlist.begin(); it != cells[i].nlist.end(); ++it)
            {
                int tmp = *it;
                int n = tmp;
                if((nets[n].unlock[from]+nets[n].lock[from]) == 1)
                    cells[i].gain++;
                if((nets[n].unlock[to]+nets[n].lock[to]) == 0)
                    cells[i].gain--;
            }
        }
    }*/

    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].number != 0)
        {
            from = cells[i].block;
            to = another(from);
            for (vector<int>::iterator it = cells[i].nlist.begin(); it != cells[i].nlist.end(); ++it)
            {
                int tmp = *it;
                int n = tmp;
                if (nets[n].count_block[from] == 1)
                    cells[i].gain++;
                if (nets[n].count_block[to] == 0)
                    cells[i].gain--;
            }
        }
    }

    tmp = 2 * pmax + 2;
    bucket[0] = new vector<int>[tmp];
    bucket[1] = new vector<int>[tmp];

    vector<int>::iterator it;
    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].number != 0)
        {
            n = cells[i].number;
            g = cells[i].gain;
            if (cells[i].block == 0)
                bucket[0][pmax - g].push_back(n);
            else
                bucket[1][pmax - g].push_back(n);
        }
    }
}

void part_graph::move()
{
    int from, to, base[2]; //base:base cell number
    int index;             //index of from block's max gain
    max_gain_sum = 0;
    int tmp_max_gain_sum = 0;
    bool done = true;
    bool flag = false;
    bool modify;
    bool nobasecell = false;
    int increase = 0;
    pass = 0;
    opt_cutsize = init_cutsize;
    vector<int>::iterator it, c;

    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].number != 0)
            free.push_front(cells[i].number);
    }

    double t1 = clock();
    while (free.size() != 0 && !nobasecell)
    {
        modify = false;
        flag = false;
        area_tmp1[0] = area_tmp2[0] = area[0];
        area_tmp1[1] = area_tmp2[1] = area[1];
        for (int i = 0; i < 2; i++) //find max gain of each side
        {
            for (int j = 0; j < pmax - pmin + 1; j++)
            {
                if (bucket[i][j].size() != 0)
                {
                    maxgain[i] = pmax - j;
                    break;
                }
            }
        }

        if (maxgain[0] >= maxgain[1]) //select the larger maxgain of two sides
            from = 0;
        else
            from = 1;
        to = another(from);

        index = pmax - maxgain[from];

        for (; index < pmax - pmin + 1; index++) //starting to find base cell in "from block"
        {
            for (it = bucket[from][index].begin(); it != bucket[from][index].end(); ++it)
            {
                int tmp = *it;
                area_tmp1[from] -= cells[tmp].size;
                area_tmp1[to] += cells[tmp].size;
                if (!balance(area_tmp1[0], area_tmp1[1]))
                {
                    area_tmp1[from] += cells[tmp].size;
                    area_tmp1[to] -= cells[tmp].size;
                }
                else
                {
                    base[from] = tmp; //base cell number in from block
                    maxgain[from] = cells[tmp].gain;
                    flag = true;
                    break;
                }
            }
            if (flag)
                break;
        }

        if (!flag) // If you can’t find a movable base cell on the from side, look for it on the other side
        {
            done = false;
            maxgain[from] = pmin;
            swap(from, to);
            index = pmax - maxgain[from];

            for (; index < pmax - pmin + 1; index++) //Start to find the base cell to be moved
            {
                for (it = bucket[from][index].begin(); it != bucket[from][index].end(); ++it)
                {
                    int tmp = *it;
                    area_tmp1[from] -= cells[tmp].size;
                    area_tmp1[to] += cells[tmp].size;
                    if (!balance(area_tmp1[0], area_tmp1[1]))
                    {
                        area_tmp1[from] += cells[tmp].size;
                        area_tmp1[to] -= cells[tmp].size;
                    }
                    else
                    {
                        base[from] = tmp;
                        maxgain[from] = cells[tmp].gain;
                        done = true;
                        break;
                    }
                }
                if (done)
                {
                    int tmp = base[from];
                    index = pmax - maxgain[from];
                    area[from] = area_tmp1[from];
                    area[to] = area_tmp1[to];
                    pass++;
                    tmp_max_gain_sum += maxgain[from];
                    free.erase(remove(free.begin(), free.end(), base[from]), free.end());
                    modify = true;
                    update_gain(tmp);
                    for (vector<int>::iterator it = cells[tmp].nlist.begin(); it != cells[tmp].nlist.end(); ++it)
                    {
                        int k = *it;
                        nets[k].unlock[from]--;
                        nets[k].lock[to]++;
                    }
                    break;
                }
            }
        }
        else
        {
            flag = false;
            index = pmax - maxgain[to];
            for (; index < pmax - pmin + 1; index++)
            {
                for (it = bucket[to][index].begin(); it != bucket[to][index].end(); ++it)
                {
                    int tmp = *it;
                    area_tmp2[to] -= cells[tmp].size;
                    area_tmp2[from] += cells[tmp].size;
                    if (!balance(area_tmp2[0], area_tmp2[1]))
                    {
                        area_tmp2[to] += cells[tmp].size;
                        area_tmp2[from] -= cells[tmp].size;
                    }
                    else
                    {
                        base[to] = tmp;
                        maxgain[to] = cells[tmp].gain;
                        flag = true;
                        break;
                    }
                }
                if (flag)
                    break;
            }

            if (flag == false)
            {
                int tmp = base[from];
                index = pmax - maxgain[from];
                area[from] = area_tmp1[from];
                area[to] = area_tmp1[to];
                pass++;
                tmp_max_gain_sum += maxgain[from];
                free.erase(remove(free.begin(), free.end(), base[from]), free.end());
                modify = true;
                update_gain(tmp);
                for (vector<int>::iterator it = cells[tmp].nlist.begin(); it != cells[tmp].nlist.end(); ++it)
                {
                    int k = *it;
                    nets[k].unlock[from]--;
                    nets[k].lock[to]++;
                }
            }
            else
            {
                if (maxgain[from] >= maxgain[to])
                { 
                    //from
                    int tmp = base[from];
                    index = pmax - maxgain[from];
                    area[from] = area_tmp1[from];
                    area[to] = area_tmp1[to];
                    pass++;
                    tmp_max_gain_sum += maxgain[from];
                    free.erase(remove(free.begin(), free.end(), base[from]), free.end());
                    modify = true;
                    update_gain(tmp);
                    for (vector<int>::iterator it = cells[tmp].nlist.begin(); it != cells[tmp].nlist.end(); ++it)
                    {
                        int k = *it;
                        nets[k].unlock[from]--;
                        nets[k].lock[to]++;
                    }
                }
                else
                { 
                    //to
                    swap(from, to);
                    int tmp = base[from];
                    index = pmax - maxgain[from];
                    area[from] = area_tmp2[from];
                    area[to] = area_tmp2[to];
                    pass++;
                    tmp_max_gain_sum += maxgain[from];
                    free.erase(remove(free.begin(), free.end(), base[from]), free.end());
                    modify = true;
                    update_gain(tmp);
                    for (vector<int>::iterator it = cells[tmp].nlist.begin(); it != cells[tmp].nlist.end(); ++it)
                    {
                        int k = *it;
                        nets[k].unlock[from]--;
                        nets[k].lock[to]++;
                    }
                }
            }
        }

        int c = compute_cutsize();

        if (opt_cutsize > c)
        {
            opt_cutsize = c;
            //max_gain_sum = tmp_max_gain_sum;
            opt_block[0].clear();
            opt_block[1].clear();
            opt_num[0] = opt_num[1] = 0;
            for (int i = 1; i <= max_cell_num; i++)
            {
                if (cells[i].number != 0)
                {
                    opt_block[cells[i].block].push_back(i);
                    opt_num[cells[i].block]++;
                }
            }
        }
        else
            increase++;

        if (done == false || modify == false)
            nobasecell = true;

        double t2 = clock();
        if (((t2 - t1) / CLOCKS_PER_SEC) > 1000 || pass > (float)(numcells * 0.75) || increase > 10)
            break;
    }
}

int part_graph::compute_cutsize()
{
    int cutsize = 0;
    for (int i = 1; i <= numnets; i++)
    {
        int side[2] = {0};
        for (vector<int>::iterator it = nets[i].clist.begin(); it != nets[i].clist.end(); ++it)
        {
            int tmp = *it;
            if (cells[tmp].block == 0)
                side[0] = 1;
            else
                side[1] = 1;
        }
        if ((side[0] == 1) && (side[1] == 1))
            cutsize++;
    }
    return cutsize;
}

void part_graph::update_gain(int inf)
{
    //cout<<"base cell c"<<inf<<endl;
    int from, to;
    from = cells[inf].block;
    to = another(from);
    cells[inf].locked = true;
    cells[inf].block = to;

    /*for(vector<int>::iterator k = cells[inf].nlist.begin(); k != cells[inf].nlist.end(); ++k)
    {
        int n = *k;
        //cout<<"net "<<n<<endl;
        if(nets[n].count_block[to] == 0)  //if T(n)=0
        {
            for(int i = 1; i <= max_cell_num; i++)
                if(cells[i].locked == false && cells[i].number != 0)
                    cells[i].gain++;
        }
        else if(nets[n].count_block[to] == 1) //else if T(n)=1
        {
            for(int i = 1; i <= max_cell_num; i++)
                if(cells[i].locked == false && cells[i].number != 0 && cells[i].block == to)
                    cells[i].gain--;
            
        }

        nets[n].count_block[from]--; //F(n) <- F(n)-1
        nets[n].count_block[to]++; //T(n) <- T(n)+1

        if(nets[n].count_block[from] == 0)  //if F(n)=0
        {
            for(int i = 1; i <= max_cell_num; i++)
                if(cells[i].locked == false && cells[i].number != 0)
                    cells[i].gain--;
        }
        else if(nets[n].count_block[from] == 1) //else if F(n)=1
        {
            for(int i = 1; i <= max_cell_num; i++)
                if(cells[i].locked == false && cells[i].number != 0 && cells[i].block == from)
                    cells[i].gain++;
        }
    }*/

    for (int i = 1; i <= max_cell_num; i++)
        cells[i].gain = 0;

    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].number != 0 && !cells[i].locked)
        {
            from = cells[i].block;
            to = another(from);
            for (vector<int>::iterator it = cells[i].nlist.begin(); it != cells[i].nlist.end(); ++it)
            {
                int tmp = *it;
                int n = tmp;
                if ((nets[n].unlock[from] + nets[n].lock[from]) == 1)
                    cells[i].gain++;
                if ((nets[n].unlock[to] + nets[n].lock[to]) == 0)
                    cells[i].gain--;
            }
        }
    }

    //clear bucket
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < pmax - pmin + 1; j++)
            bucket[i][j].clear();

    //cout << "begin"<<endl;
    for (list<int>::iterator it = free.begin(); it != free.end(); ++it)
    {
        int n = *it; //cell number on free list
        int g = cells[n].gain;
        int b = cells[n].block;
        bucket[b][pmax - g].push_back(n);
    }
    //cout<<"end"<<endl;
}

void part_graph::printcells()
{
    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].size != 0)
            cout << "c" << cells[i].number << " " << cells[i].size << endl;
    }

    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].number != 0)
        {
            cout << "Cell c" << i << " { ";
            for (vector<int>::iterator it = cells[i].nlist.begin(); it != cells[i].nlist.end(); ++it)
            {
                int tmp = *it;
                cout << "n" << tmp << " ";
            }
            cout << "}" << endl;
        }
    }

    printf("Cell\tPart\tGain\n");
    for (int i = 1; i <= max_cell_num; i++)
    {
        if (cells[i].number != 0)
            printf("%4d\t%4d\t%4d\n", cells[i].number, cells[i].block, cells[i].gain);
    }
}

void part_graph::printnets()
{
    for (int i = 1; i <= numnets; i++)
    {
        cout << "NET n" << i << " { ";
        for (vector<int>::iterator it = nets[i].clist.begin(); it != nets[i].clist.end(); ++it)
        {
            int tmp = *it;
            cout << "c" << tmp << " ";
        }
        cout << "}" << endl;
    }

    //for(int i = 1; i <= numnets; i++)
    //    printf("NET n%d : unlock[0]=%d,unlock[1]=%d\n", i, nets[i].unlock[0], nets[i].unlock[1]);
}

void part_graph::print_result()
{
    cout << "cut_size " << opt_cutsize << endl;
    cout << endl;
    /*cout<<"A "<<opt_num[0]<<endl;
    for(vector<int>::iterator it = opt_block[0].begin(); it != opt_block[0].end(); ++it)
    {
        int tmp = *it;
        cout<<"c"<<tmp<<endl;
    }
    cout<<"B "<<opt_num[1]<<endl;
    for(vector<int>::iterator it = opt_block[1].begin(); it != opt_block[1].end(); ++it)
    {
        int tmp = *it;
        cout<<"c"<<tmp<<endl;
    }*/
}
