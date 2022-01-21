#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <set>
#include <random>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <list>
#include <map>
#include "Cell.h"
#include "Net.h"

using namespace std;

typedef struct FM
{
    map<int, Cell> Cells;
    map<int, Net> Nets;
    map<int, vector<int> > A_Bucket, B_Bucket, Bucket;
    vector<int> Result;
    int lock_count;
    int c_size; // # of cells
    int n_size; // # of nets;
    int A_size;
    int B_size;
    int limit; // # of cells in the larger partition
    int Pmax;
    int cutsize;
    int Total_Gain;
} FM;

random_device rnd;
default_random_engine gen = default_random_engine(rnd());
uniform_real_distribution<> rand_dis(0.0, 1.0);

time_t begin_time;
int B_full = 0;
map<int, set<unsigned int> > A_buck, B_buck;
unsigned int N_net, N_cell;
int A_max = 0, B_max = 0;
long move_A = 0, move_B = 0, in_A = 0, in_B = 0;
long pMax = 0;

FM ReadFile(string filename);
void PrintAll(FM myfm);
void GetGain(FM &myfm);
void GetCut(FM &myfm);
void UpdateGain(int cur, FM &myfm);
void UpdateBucket(int c, int pre_g, FM &myfm);
void Read_File(string fileName);
void FM_Start();
void initial_gain();
void From_A();
void From_B();

void MoveCells(FM &myfm);

void FS_TE(int c, FM myfm, int &FS, int &TE);
bool All_Locked(FM myfm);
bool Is_Balance(int c, FM myfm);
vector<set<unsigned int> > Nets;
vector<set<unsigned int> > Cells;
vector<unsigned int> net_A, net_B;
vector<unsigned int> Tmp_Cell;

vector<bool> Check;
vector<int> gains;
vector<bool> res;
set<unsigned int> A, B;

int GetPmax(FM myfm);

FM ReadFile(string filename)
{
    srand(time(NULL));
    begin_time = time(NULL);

    Read_File(filename);
    FM_Start();
    ofstream fout("output.txt");
    for (int i = 0; i < N_cell; ++i)
    {
        if (res[i])
            fout << "0";
        else
            fout << "1";
        fout << endl;
    }
    fout.close();
    return FM();

    ifstream infile;
    infile.open(filename, ios::in);
    if (!infile.is_open())
    {
        cout << "Cannot Open File " << filename << endl;
        return FM();
    }

    // Successfully Open File
    FM myfm;
    string str;
    stringstream ss;

    infile >> myfm.n_size >> myfm.c_size;

    for (int i = 0; i <= myfm.c_size; i++)
    {
        In_Part(myfm.Cells[i]) = -1;
        myfm.Result.push_back(-1);
    }
    // Initialization
    myfm.A_size = 0;
    myfm.B_size = 0;
    myfm.lock_count = 0;
    myfm.Total_Gain = 0;
    myfm.limit = ceil(myfm.c_size * 0.549);

    // Read Each Net
    int n, current_net = 1; // Careful! Not use the number 0
    ;
    getline(infile, str);
    int in_A = 0;
    while (getline(infile, str))
    {
        // int in_A = 0, in_B = 0;
        A(myfm.Nets[current_net]) = 0; // initialize this nets
        B(myfm.Nets[current_net]) = 0;
        // traverse the cells in this net
        ss.clear();
        ss.str(str);
        int rnd = rand() % 2;
        // cout << "str: " << str << endl;
        while (ss >> n)
        {
            // Add the cells in net to vector, the net to cells.
            Links(myfm.Nets[current_net]).push_back(n);
            In_Nets(myfm.Cells[n]).push_back(current_net);
            // cout << n << " in net " << current_net << endl;

            // Careful! Not use the number 0
            if (In_Part(myfm.Cells[n]) == -1) // doesn't belong to any part
            {
                myfm.Cells[n].is_locked = 0;
                // int rnd = 1;
                // A is oversize
                if (myfm.A_size >= myfm.limit - 1)
                {
                    // Add current cell to part B (=1)
                    In_Part(myfm.Cells[n]) = 1;
                    myfm.B_size += 1;
                    myfm.Result[n] = 1;
                    myfm.Nets[current_net].in_B += 1;
                }
                else if (myfm.B_size >= myfm.limit - 1)
                {
                    // Add current cell to part A (=0)
                    In_Part(myfm.Cells[n]) = 0;
                    myfm.A_size += 1;
                    myfm.Result[n] = 0;
                    myfm.Nets[current_net].in_A += 1;
                }
                else
                {
                    In_Part(myfm.Cells[n]) = rnd;
                    if (rnd == 1)
                    {
                        myfm.B_size += 1;
                        myfm.Result[n] = 1;
                        myfm.Nets[current_net].in_B += 1;
                    }

                    else
                    {
                        myfm.A_size += 1;
                        myfm.Result[n] = 0;
                        myfm.Nets[current_net].in_A += 1;
                        // cout << " c" << n << " Add to A' in n" << current_net << " now =  " << myfm.Result[n] << endl;
                    }
                }
                // cout << "Cell " << n << " in " << In_Part(myfm.Cells[n]) << endl;
            }
            else if (In_Part(myfm.Cells[n]) == 0)
            {
                myfm.Nets[current_net].in_A += 1;
            }
            else
            {
                myfm.Nets[current_net].in_B += 1;
            }
        }
        current_net += 1;
        // PrintAll(myfm);
    }

    // for (int i = 1; i <= myfm.c_size; i++)
    // {
    //     if (In_Part(myfm.Cells[i]) == -1)
    //     {
    //         // cout << "Cell: " << i << " doesn't connect to a net" << endl;
    //         if (myfm.A_size >= myfm.limit - 1)
    //         {
    //             // Add current cell to part B (=1)
    //             In_Part(myfm.Cells[n]) = 1;
    //             myfm.B_size += 1;
    //             for (int net = 0; net < myfm.Cells[n].in_net.size(); net++)
    //             {
    //                 int tmp_net = myfm.Cells[n].in_net[net];
    //                 myfm.Nets[tmp_net].in_B += 1;
    //             }
    //             myfm.Result[i] = 1;
    //         }
    //         else
    //         {
    //             // Add current cell to part A (=0)
    //             In_Part(myfm.Cells[n]) = 0;
    //             myfm.A_size += 1;
    //             for (int net = 0; net < myfm.Cells[n].in_net.size(); net++)
    //             {
    //                 int tmp_net = myfm.Cells[n].in_net[net];
    //                 myfm.Nets[tmp_net].in_A += 1;
    //             }
    //             myfm.Result[i] = 0;
    //         }
    //     }
    // }

    // for (n = 1; n <= myfm.n_size; n++)
    // {
    //     for (int c = 0; c < myfm.Nets[n].in_cell.size(); c++)
    //     {
    //         int cell = myfm.Nets[n].in_cell[c];
    //         if (myfm.Cells[cell].in_part == 0)
    //         {
    //             myfm.Nets[n].in_A += 1;
    //         }
    //         else if (myfm.Cells[cell].in_part == 1)
    //         {
    //             myfm.Nets[n].in_B += 1;
    //         }
    //     }
    // }
    // cout << "Pre Process Time: " << time(NULL) - begin_time << endl;
    // myfm.Pmax = GetPmax(myfm);
    //  cout << "Finish Initialization " << endl;
    GetGain(myfm);
    // cout << "Get Gain Time: " << time(NULL) - begin_time << endl;
    GetCut(myfm);
    // cout << "Get Cut Time: " << time(NULL) - begin_time << endl;
    MoveCells(myfm);

    return myfm;
}

void Read_File(string fileName)
{
    ifstream fin(fileName);
    string str;
    getline(fin, str);
    istringstream ss(str);
    ss >> N_net >> N_cell;

    Nets.resize(N_net);
    net_A.resize(N_net, 0);
    net_B.resize(N_net, 0);
    Cells.resize(N_cell);
    res.resize(N_cell);
    Check.resize(N_cell, false);

    unsigned int num;

    for (int i = 0; i < N_net; i++)
    {
        getline(fin, str);
        istringstream ss(str);
        while (ss >> num)
        {
            num += -1;
            pMax += 1;
            Nets[i].insert(num);
            Cells[num].insert(i);
            if (!Check[num])
            {
                if (!((rand_dis(gen) < 0.5 && in_A < (0.5 * N_cell)) || in_B >= (0.5 * N_cell)))
                {
                    B.insert(num);
                    res[num] = 1;
                    in_B += 1;
                }
                else
                {
                    A.insert(num);
                    res[num] = 0;
                    in_A += 1;
                }
            }
            if (res[num] == 0)
                net_A[i] += 1;
            else
                net_B[i] += 1;
            Check[num] = true;
        }
    }
    fin.close();
}

void GetGain(FM &myfm)
{
    myfm.A_Bucket.clear();
    myfm.B_Bucket.clear();

    int FS = 0, TE = 0;

    for (int c = 1; c <= myfm.c_size; c++)
    {
        FS = 0;
        TE = 0;
        // FS_TE(i, myfm, FS, TE);
        int same_part, n, part = In_Part(myfm.Cells[c]);

        // in_net and in_cell from 0 to size -1
        for (int i = 0; i < myfm.Cells[c].in_net.size(); i++)
        {
            n = myfm.Cells[c].in_net[i];
            same_part = 0;

            // if (n < B_full - 2)
            // {
            //     // All in B
            //     if (part == 1)
            //     {
            //         TE += 1;
            //     }
            // }
            // else if (n > B_full + 2)
            // {
            //     // All in A
            //     if (part == 0)
            //     {
            //         if (myfm.Nets[n].in_B == 0)
            //         {
            //             TE += 1;
            //         }
            //     }
            // }

            if (part == 0)
            {
                if (myfm.Nets[n].in_A == 1)
                {
                    FS += 1;
                }
                if (myfm.Nets[n].in_B == 0)
                {
                    TE += 1;
                }
            }
            else if (part == 1)
            {
                if (myfm.Nets[n].in_B == 1)
                {
                    FS += 1;
                }
                if (myfm.Nets[n].in_A == 0)
                {
                    TE += 1;
                }
            }
        }
        myfm.Cells[c].gain = FS - TE;
        // Update Bucket
        // if (myfm.Cells[i].in_part == 0)
        // {
        //     myfm.A_Bucket[FS - TE].push_back(i);
        // }
        // else if (myfm.Cells[i].in_part == 1)
        // {
        //     myfm.B_Bucket[FS - TE].push_back(i);
        // }
        myfm.Bucket[FS - TE].push_back(c);
        // cout << "Add Bucket: " << FS << "-" << TE << ", c" << c << endl;
    }
}
void FS_TE(int c, FM myfm, int &FS, int &TE)
{

    int same_part, n;
    // in_net and in_cell from 0 to size -1
    for (int i = 0; i < myfm.Cells[c].in_net.size(); i++)
    {
        int part = In_Part(myfm.Cells[c]);
        n = myfm.Cells[c].in_net[i];
        same_part = 0;

        if (n < B_full - 2)
        {
            // All in B
            if (part == 1)
            {
                TE += 1;
            }
        }
        else if (n > B_full + 2)
        {
            // All in A
            if (part == 0)
            {
                if (myfm.Nets[n].in_B == 0)
                {
                    TE += 1;
                }
            }
        }

        if (part == 0)
        {
            if (myfm.Nets[n].in_A == 1)
            {
                FS += 1;
            }
            if (myfm.Nets[n].in_B == 0)
            {
                TE += 1;
            }
        }
        else if (part == 1)
        {
            if (myfm.Nets[n].in_B == 1)
            {
                FS += 1;
            }
            if (myfm.Nets[n].in_A == 0)
            {
                TE += 1;
            }
        }

        // for (int j = 0; j < myfm.Nets[n].in_cell.size(); j++)
        // {
        //     if (part == In_Part(myfm.Cells[myfm.Nets[n].in_cell[j]]) && c != myfm.Nets[n].in_cell[j])
        //     {
        //         // cout << "c" << c << ", c" << myfm.Nets[n].in_cell[j] << " same part -> " << "n" << n << endl;
        //         same_part += 1;
        //     }
        // }
        // if (same_part == 0)
        // {
        //     // cout << "c" << c << " in n" << n << " FS" << endl;
        //     FS += 1;
        // }
        // else if (same_part == myfm.Nets[n].in_cell.size() - 1)
        // {
        //     // cout << "c" << c << " in n" << n << " TE" << endl;
        //     TE += 1;
        // }
    }
}
int GetPmax(FM myfm)
{
    int res = 0;
    for (int i = 1; i <= myfm.c_size; i++)
    {
        int n = min(int(In_Nets(myfm.Cells[i]).size()), myfm.limit);
        if (res < n)
            res = n;
    }
    // cout << "PMax " << res << endl;
    return res;
}
void FM_Start()
{
    time_t start, end;
    double TT;
    double dd;
    bool flag = false;
    start = time(NULL);
    while (1)
    {

        initial_gain();
        long Gain_maxD = 0, Gain_D = 0;
        vector<long> gainDelta(N_cell);
        int pos = 0;
        for (int i = 0; i < N_cell; i++)
        {
            if (B_max >= A_max)
            {
                if (((in_A + 1) <= 0.55 * N_cell && (in_B - 1) >= 0.45 * N_cell && move_B > 0) || move_A == 0)
                {
                    // cout << "B" << endl;
                    From_B();
                }
                else
                {
                    // cout << "A" << endl;
                    From_A();
                }

            } // Remove B
            else
            {
                if (((in_B + 1) <= 0.55 * N_cell && (in_A - 1) >= 0.45 * N_cell && move_A > 0) || move_B == 0)
                {
                    // cout << "A" << endl;
                    From_A();
                }
                else
                {
                    // cout << "B" << endl;
                    From_B();
                }
            }

            Gain_D += gains[Tmp_Cell.back()];
            // cout << "Gain_D " << Gain_D << "\n";
            if (Gain_D > Gain_maxD)
            {
                Gain_maxD = Gain_D;
                pos = i;
            }
        }
        // cout << Gain_maxD << endl;
        if (Gain_maxD <= 0)
            return;
        else
        {
            for (int i = 0; i < N_cell; ++i)
            {
                // cout << "k " << pos << endl;
                if (i <= pos)
                {
                    // cout << "y";
                    if (res[Tmp_Cell[i]] == 1)
                    {
                        A.erase(Tmp_Cell[i]);
                        B.insert(Tmp_Cell[i]);
                    }
                    else
                    {
                        B.erase(Tmp_Cell[i]);
                        A.insert(Tmp_Cell[i]);
                    }
                }
                else
                {
                    // revert change
                    res[Tmp_Cell[i]] = (res[Tmp_Cell[i]]) ? 0 : 1;
                    for (auto it = Cells[Tmp_Cell[i]].begin(); it != Cells[Tmp_Cell[i]].end(); ++it)
                    {

                        if (res[Tmp_Cell[i]] == 0)
                        {
                            net_B[*it] += -1;
                            net_A[*it] += 1;
                        }
                        else
                        {
                            net_B[*it] += 1;
                            net_A[*it] += -1;
                        }
                    }
                }
            }
        }
        end = time(NULL);
        dd = difftime(end, start);
        if (!flag)
        {
            TT = dd + 5.0;
            flag = true;
        }
        if ((29.5 - dd) < TT)
        {
            return;
        }
    }
}

void initial_gain()
{
    Tmp_Cell.clear();

    gains.clear();
    gains.resize(N_cell);

    Check.clear();
    Check.resize(N_cell, 0);

    A_buck.clear();
    B_buck.clear();
    A_max = -pMax;
    B_max = -pMax;

    in_A = A.size();
    move_A = A.size();
    in_B = B.size();
    move_B = B.size();

    bool left;
    int gain;
    for (unsigned int i = 0; i < (N_cell); ++i)
    {
        gain = 0;
        // check in set A or B
        left = (res[i] == 0) ? true : false;
        // *it => net, iterate nets that connecting this cell, use netlist left/right num to calculate gain
        for (auto it = Cells[i].begin(); it != Cells[i].end(); ++it)
        {

            if (left)
            {
                if (net_A[*it] == 1)
                    gain += 1;

                else if (net_B[*it] == 0)
                    gain += -1;

                // cout << "Left Gain" << gain << endl;
            }
            else
            {
                if (net_B[*it] == 1)
                    gain += 1;
                else if (net_A[*it] == 0)
                    gain += -1;
                // cout << "Right Gain" << gain << endl;
            }
        }

        if (left)
        {
            A_max = max(A_max, gain);
            A_buck[gain].insert(i);
            gains[i] = gain;
        }
        else
        {
            B_max = max(B_max, gain);
            B_buck[gain].insert(i);
            gains[i] = gain;
        }
    }
    // cout << "Max Gain" << A_max << " " << B_max << endl;
}
void From_A()
{
    unsigned int selectedCell = *A_buck[A_max].begin();
    Check[selectedCell] = true; // set moved cell locked
    A_buck[A_max].erase(A_buck[A_max].begin());
    // iterate netlist connecting to the selected cell
    for (auto it = Cells[selectedCell].begin(); it != Cells[selectedCell].end(); ++it)
    {
        unsigned int currentNet = *it;
        unsigned int beforeMoveLeft =
            net_A[currentNet];
        unsigned int beforeMoveRight = net_B[currentNet];
        net_A[currentNet] += -1;
        net_B[currentNet] += 1;
        // Update cell gains before move
        // iterate cells on this netlist
        for (auto _it = Nets[*it].begin(); _it != Nets[*it].end(); ++_it)
        {
            unsigned int currentCell = *_it;
            if (Check[currentCell])
                continue; // if cell is locked
            // increase gain on left if right is empty
            if (beforeMoveRight == 0)
            {
                A_buck[gains[currentCell]].erase(currentCell);
                if (gains[currentCell] == A_max)
                    A_max += 1;
                gains[currentCell] += 1;
                A_buck[gains[currentCell]].insert(currentCell);

            } // decrease gain of the only right cell
            else if (beforeMoveRight == 1 && res[currentCell] == 1)
            {
                B_buck[gains[currentCell]].erase(currentCell);
                if (gains[currentCell] == B_max && B_buck[gains[currentCell]].empty())
                    B_max += -1;
                gains[currentCell] += -1;
                B_buck[gains[currentCell]].insert(currentCell);
            }
            // decrease all cells on the right if left = 0 after moving
            if (net_A[currentNet] == 0)
            {
                B_buck[gains[currentCell]].erase(currentCell);
                if (gains[currentCell] == B_max && B_buck[gains[currentCell]].empty())
                    B_max += -1;
                gains[currentCell] += -1;
                B_buck[gains[currentCell]].insert(currentCell);

            } // increase only cell on left after moving if left = 1 afer moving
            else if (net_A[currentNet] == 1 && res[currentCell] == 0)
            {
                A_buck[gains[currentCell]].erase(currentCell);
                if (gains[currentCell] == A_max)
                    A_max += 1;
                gains[currentCell] += 1;
                A_buck[gains[currentCell]].insert(currentCell);
            }
        }
    }
    // update data structures
    res[selectedCell] = 1;
    Tmp_Cell.push_back(selectedCell);
    in_A += -1;
    in_B += 1;
    move_A += -1;
    while (A_buck[A_max].empty() && move_A != 0)
    {
        // cout << move_A << " " << A_max << endl;
        A_max += -1;
    }
    if (move_A == 0)
    {
        A_max = -pMax - 1;
    }
}
void From_B()
{
    unsigned int selectedCell = *B_buck[B_max].begin();
    Check[selectedCell] = true; // set moved cell locked
    B_buck[B_max].erase(B_buck[B_max].begin());
    // iterate netlist connecting to the selected cell
    for (auto it = Cells[selectedCell].begin(); it != Cells[selectedCell].end(); ++it)
    {
        unsigned int currentNet = *it;
        unsigned int beforeMoveLeft = net_A[currentNet];
        unsigned int beforeMoveRight = net_B[currentNet];
        net_A[currentNet] += 1;
        net_B[currentNet] += -1;
        // Update cell gains before move
        // iterate cells on this netlist
        for (auto _it = Nets[*it].begin(); _it != Nets[*it].end(); ++_it)
        {
            unsigned int currentCell = *_it;
            if (Check[currentCell])
                continue; // if cell is locked
            if (beforeMoveLeft == 0)
            {
                B_buck[gains[currentCell]].erase(currentCell);
                if (gains[currentCell] == B_max)
                    B_max += 1;
                gains[currentCell] += 1;
                B_buck[gains[currentCell]].insert(currentCell);

            } // decrease gain of the only left cell
            else if (beforeMoveLeft == 1 && res[currentCell] == 0)
            {
                A_buck[gains[currentCell]].erase(currentCell);
                if (gains[currentCell] == A_max && A_buck[gains[currentCell]].empty())
                    A_max += -1;
                gains[currentCell] += -1;
                A_buck[gains[currentCell]].insert(currentCell);
            }
            // decrease all cells on the left if right = 0 after moving
            if (net_B[currentNet] == 0)
            {
                A_buck[gains[currentCell]].erase(currentCell);
                if (gains[currentCell] == A_max && A_buck[gains[currentCell]].empty())
                    A_max += -1;
                gains[currentCell] += -1;
                A_buck[gains[currentCell]].insert(currentCell);

            } // increase only cell on right after moving if right = 1 after moving
            else if (net_B[currentNet] == 1 && res[currentCell] == 1)
            {
                B_buck[gains[currentCell]].erase(currentCell);
                if (gains[currentCell] == B_max)
                    B_max += 1;
                gains[currentCell] += 1;
                B_buck[gains[currentCell]].insert(currentCell);
            }
        }
    }

    // update data structures
    res[selectedCell] = 0;
    Tmp_Cell.push_back(selectedCell);
    in_B += -1;
    in_A += 1;
    move_B += -1;
    while (B_buck[B_max].empty() && move_B != 0)
    {
        // cout << move_B << " " << B_max << endl;
        B_max += -1;
    }
    if (move_B == 0)
    {
        B_max = -pMax - 1;
    }
}

void PrintAll(FM myfm)
{
    cout << "===== Cells =====" << endl;
    for (int i = 1; i <= myfm.c_size; i++)
    {
        cout << i << ": " << In_Part(myfm.Cells[i]) << " => ";
        for (int j = 0; j < myfm.Cells[i].in_net.size(); j++)
        {
            cout << "n" << myfm.Cells[i].in_net[j] << " ";
        }
        cout << endl;
    }

    cout << "===== Nets =====" << endl;
    for (int i = 1; i <= myfm.n_size; i++)
    {
        cout << "n" << i << ": ";
        for (int j = 0; j < myfm.Nets[i].in_cell.size(); j++)
        {
            cout << "c" << myfm.Nets[i].in_cell[j] << " ";
        }
        cout << endl;
        cout << "(A, B) " << myfm.Nets[i].in_A << ", " << myfm.Nets[i].in_B << endl;
    }
    cout << "===== Buckets =====" << endl;
    // cout << "Bucket A: " << myfm.A_Bucket.size() << endl;
    // for (map<int, vector<int> >::const_iterator it = myfm.A_Bucket.begin(); it != myfm.A_Bucket.end(); ++it)
    // {
    //     vector<int> tmp = it->second;
    //     cout << "gain (" << it->first << ") :";
    //     for (int j = 0; j < tmp.size(); j++)
    //     {
    //         cout << "c" << tmp[j] << " ";
    //     }
    //     cout << endl;
    // }

    // // cout << endl << "Max value: " << id->first << endl << endl;
    // cout << endl
    //      << "Bucket B: " << myfm.B_Bucket.size() << endl;
    // for (map<int, vector<int> >::const_iterator it = myfm.B_Bucket.begin(); it != myfm.B_Bucket.end(); ++it)
    // {
    //     vector<int> tmp = it->second;
    //     cout << "gain (" << it->first << ") :";
    //     for (int j = 0; j < tmp.size(); j++)
    //     {
    //         cout << "c" << tmp[j] << " ";
    //     }
    //     cout << endl;
    // }

    cout << "Total Bucket: " << myfm.Bucket.size() << endl;
    for (map<int, vector<int> >::const_iterator it = myfm.Bucket.begin(); it != myfm.Bucket.end(); ++it)
    {
        vector<int> tmp = it->second;
        cout << "gain (" << it->first << ") :";
        for (int j = 0; j < tmp.size(); j++)
        {
            cout << "c" << tmp[j] << " ";
        }
        cout << endl;
    }

    cout << endl;

    cout << "PMax = " << myfm.Pmax << endl;
    cout << "Total (A, B) " << myfm.A_size << ", " << myfm.B_size << endl;

    cout << "Cut Size = " << myfm.cutsize << endl;

    cout << "======== Result ======" << endl;
    for (int i = 1; i <= myfm.c_size; i++)
    {
        cout << "c" << i << " in " << myfm.Result[i] << endl;
    }

    cout << endl;
    cout << "Check Locked: " << All_Locked(myfm) << endl;
}
void GetCut(FM &myfm)
{
    int result_cut = 0;
    for (int i = 1; i <= myfm.n_size; i++)
    {
        bool found_cut = false;
        // Check first cells in this net
        for (int j = 0; j < myfm.Nets[i].in_cell.size(); i++)
        {
            for (int k = j + 1; k < myfm.Nets[i].in_cell.size(); k++)
            {
                int cell_a = myfm.Nets[i].in_cell[j];
                int cell_b = myfm.Nets[i].in_cell[k];
                if (myfm.Cells[cell_a].in_part != myfm.Cells[cell_b].in_part)
                {
                    result_cut += 1;
                    found_cut = true;
                    break;
                }
            }
            if (found_cut)
                break;
        }
    }
    myfm.cutsize = result_cut;
}
void MoveCells(FM &myfm)
{

    // Get the maximum gain for the last member in vector
    map<int, vector<int> >::iterator id = myfm.Bucket.end();
    id--;
    int current_cell, count, current_cut = myfm.cutsize;
    // Iteration till all the cell is locked
    while (myfm.lock_count < myfm.c_size)
    {

        bool stop = false;
        // cout << "=========== New Iteration =========" << endl;
        // PrintAll(myfm);
        if (myfm.Bucket.empty())
            break;
        current_cell = id->second.back();
        count = 0;
        // current_cell = *(&id->second.back() - count);
        // cout << count << " == Check: " << current_cell << endl;
        // cout << "Locked: " << myfm.Cells[current_cell].is_locked << ", Balanced: " << Is_Balance(current_cell, myfm) << endl;
        if (time(NULL) - begin_time > 29)
        {
            // cout << "Time_Out" << endl;
            return;
        }

        // Choose the one isn't locked and is balanced
        // cout << "=== " << abs(myfm.A_size - myfm.B_size) << " < " << myfm.c_size * 0.1 << endl;
        int legalc = myfm.B_size - myfm.A_size;
        if (myfm.A_size > myfm.B_size)
        {
            legalc = myfm.A_size - myfm.B_size;
        }
        while (myfm.Cells[current_cell].is_locked == 1 || !Is_Balance(current_cell, myfm))
        {
            if (time(NULL) - begin_time > 29)
            {
                return;
            }
            if (myfm.Bucket.empty())
                break;
            if (current_cell == id->second.front())
            {
                id--;
                count = 0;
                stop = true;
                break;
                // continue;
            }
            count++;
            current_cell = *(&id->second.back() - count);
        }

        if (stop)
            continue;

        int belong = myfm.Cells[current_cell].in_part;
        myfm.Cells[current_cell].is_locked = 1;
        myfm.lock_count += 1;

        // cout << "erase: " << *(id->second.end() - count - 1) << ", " << current_cell << endl;
        id->second.erase(id->second.end() - count - 1);
        if (id->second.empty())
            myfm.Bucket.erase(id);

        // get current cut
        current_cut += -myfm.Cells[current_cell].gain;

        // get part

        if (belong == 0)
        {
            myfm.A_size += -1;
            myfm.B_size += 1;
        }
        else
        {
            myfm.A_size += 1;
            myfm.B_size += -1;
        }
        // Change part
        myfm.Cells[current_cell].in_part = (myfm.Cells[current_cell].in_part + 1) % 2;

        // Calculate total gain
        myfm.Total_Gain += myfm.Cells[current_cell].gain;

        // Update gain for rest of the cell
        UpdateGain(current_cell, myfm);

        if (current_cut < myfm.cutsize)
        {
            myfm.cutsize = current_cut;
            for (int ids = 1; ids <= myfm.c_size; ids++)
            {
                myfm.Result[ids] = myfm.Cells[ids].in_part;
                // cout<<answer[i]<<" ";
            }
        }

        if (!myfm.Bucket.empty())
        {
            id = myfm.Bucket.end();
            id--;
        }
    }
}
void UpdateGain(int cur, FM &myfm)
{
    int n, c;  // n for net, c for cell
    int pre_g; // previous gain
    // For all nets link to the cut cell
    int pre_part = (myfm.Cells[cur].in_part + 1) % 2;

    for (int net = 0; net < myfm.Cells[cur].in_net.size(); net++)
    {
        n = myfm.Cells[cur].in_net[net];

        if (pre_part == 0)
        {
            // Part A
            myfm.Cells[cur].from = myfm.Nets[n].in_A;
            myfm.Cells[cur].to = myfm.Nets[n].in_B;
        }
        else if (pre_part == 1)
        {
            // Part B
            myfm.Cells[cur].from = myfm.Nets[n].in_B;
            myfm.Cells[cur].to = myfm.Nets[n].in_A;
        }

        if (myfm.Cells[cur].to == 0)
        {
            // For all cells within the net connect to the cut cell
            for (int cell = 0; cell < myfm.Nets[n].in_cell.size(); cell++)
            {
                c = myfm.Nets[n].in_cell[cell];
                if (myfm.Cells[c].is_locked == 0)
                {
                    pre_g = myfm.Cells[c].gain;
                    myfm.Cells[c].gain += 1;
                    // Update bucket
                    UpdateBucket(c, pre_g, myfm);
                }
            }
        }
        else if (myfm.Cells[cur].to == 1)
        {

            for (int cell = 0; cell < myfm.Nets[n].in_cell.size(); cell++)
            {
                c = myfm.Nets[n].in_cell[cell];
                if (myfm.Cells[cur].in_part == myfm.Cells[c].in_part)
                {
                    if (myfm.Cells[c].is_locked == 0)
                    {
                        pre_g = myfm.Cells[c].gain;
                        myfm.Cells[c].gain += -1;
                        UpdateBucket(c, pre_g, myfm);
                        break;
                    }
                }
            }
        }

        myfm.Cells[cur].from += -1;
        myfm.Cells[cur].to += 1;

        if (pre_part == 0)
        {
            myfm.Nets[n].in_B = myfm.Cells[cur].to;
            myfm.Nets[n].in_A = myfm.Cells[cur].from;
        }
        else if (pre_part == 1)
        {
            myfm.Nets[n].in_A = myfm.Cells[cur].to;
            myfm.Nets[n].in_B = myfm.Cells[cur].from;
        }

        if (myfm.Cells[cur].from == 0)
        {
            for (int cell = 0; cell < myfm.Nets[n].in_cell.size(); cell++)
            {
                c = myfm.Nets[n].in_cell[cell];
                if (myfm.Cells[c].is_locked == 0)
                {
                    pre_g = myfm.Cells[c].gain;
                    myfm.Cells[c].gain += -1;
                    // Update bucket
                    UpdateBucket(c, pre_g, myfm);
                }
            }
        }
        else if (myfm.Cells[cur].from == 1)
        {
            for (int cell = 0; cell < myfm.Nets[n].in_cell.size(); cell++)
            {
                c = myfm.Nets[n].in_cell[cell];
                if (pre_part == myfm.Cells[c].in_part)
                {
                    if (myfm.Cells[c].is_locked == 0)
                    {
                        pre_g = myfm.Cells[c].gain;
                        myfm.Cells[c].gain += 1;
                        UpdateBucket(c, pre_g, myfm);
                        break;
                    }
                }
            }
        }
    }
}
void UpdateBucket(int c, int pre_g, FM &myfm)
{
    int new_g = myfm.Cells[c].gain;
    vector<int>::iterator rm = remove(myfm.Bucket[pre_g].begin(), myfm.Bucket[pre_g].end(), c);
    myfm.Bucket[pre_g].erase(rm, rm + 1);

    if (myfm.Bucket[pre_g].empty())
    {
        myfm.Bucket.erase(pre_g);
    }

    myfm.Bucket[new_g].push_back(c);
}
bool All_Locked(FM myfm)
{
    for (int i = 1; i <= myfm.c_size; i++)
    {
        if (!myfm.Cells[i].is_locked)
        {
            return false;
        }
    }
    return true;
}
bool Is_Balance(int c, FM myfm)
{

    int tmp_A = myfm.A_size;
    int tmp_B = myfm.B_size;
    if (abs((tmp_A - tmp_B)) < 0.099 * myfm.c_size)
        return true;
    // else if (abs((tmp_A - tmp_B)) > 0.101 * myfm.c_size)
    //     return false;
    if (myfm.Cells[c].in_part == 0)
    {
        tmp_A += -1;
        tmp_B += 1;
    }
    else
    {
        tmp_A += 1;
        tmp_B += -1;
    }
    if (tmp_A <= myfm.limit && tmp_A >= myfm.c_size - myfm.limit)
    {
        return true;
    }
    return false;
}
