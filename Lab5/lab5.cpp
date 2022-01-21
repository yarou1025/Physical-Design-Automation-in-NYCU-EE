#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <cstring>
#include <sstream>
#include <random>
#include <fstream>
#include <iomanip>
#include <climits>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>

using namespace std;

// ------------- Structures ---------------------
struct col
{
    int top;
    int bot;
};

struct net
{
    net()
    {
        rr = -1;
        ll = -1;
        valid = false;
    };
    bool valid;
    int ll, rr;
};

struct row
{
    int hor; // row
    int tmp;
    int ver; // column
    int next;
};

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    int n;
    vector<Point> point;
    int weight;
    bool selected;
    int track;
} Net;
//---------------- Functions --------------------
void ReadInput(string filename);
void WriteOutput();
void GreedyRoute();
void InitLUT();
void Print();
int PostProcess();
bool Finished(int c, int sig);
bool Complete();
int Route(int i);
void A(int c, int top, int bot, bool &t_f, bool &b_f);
void E(int c, int top, int bot, bool t_f, bool b_f);
void B(int c, int top, int bot);
void C(int c);
void D(int c);
void F(int c);

int Route();

//---------------- Variables -------------------
vector<col> cols;
vector<row> rows;
vector<vector<int> > LUT;
vector<vector<int> > res_ver, res_hor;
vector<vector<int> > out_ver, out_hor;

map<int, net> nets;
set<int> nets_set;
int Length, Density = 0, MaxNum = 0, SNC, Min_Jog = 1;
string filename;
random_device rnd;
default_random_engine eng(rnd());
time_t t_1, t_2;
int t_limit = 40;

int main(int argc, char *argv[])
{
    ReadInput(argv[1]);
    filename = argv[2];
    GreedyRoute();
    return 0;
}
// Global Variable
int length, total_track;
int BB = 1000, K = 3;
vector<Net> NN;
vector<int> den, Top, Bottom, vertical;
void Print_Den()
{
    for (int i = 0; i < length; i++)
    {
        cout << den[i] << " ";
    }
    cout << endl;
}
void Print_Net()
{
    for (int i = 0; i < NN.size(); i++)
    {
        // cout<<"N:"<<NN[i].n<<" ("<<NN[i].point[0].x<<","<<NN[i].point[NN[i].point.size() -1].x<<")///";
        cout << "N:" << NN[i].n << " ";
        for (int j = 0; j < NN[i].point.size(); j++)
        {
            cout << "(" << (NN[i].point[j]).x << "," << (NN[i].point[j]).y << ") ";
        }

        cout << endl;
        cout << "Weight:" << NN[i].weight << " " << endl;
    }
}
void Print_Map()
{
    cout << "---------Track" << total_track << "----------" << endl;
    for (int i = 0; i < Top.size(); i++)
    {
        cout << Top[i] << " ";
    }
    cout << endl;
    /*for(int i = 0; i < NN.size(); i++){
        int num = NN[i].point[0].x;
        while(num != 0){
            cout<<"  ";
            num--;
        }

        for(int j = NN[i].point[0].x; j <= NN[i].point[NN[i].point.size() -1].x; j++){
            cout<<NN[i].n<<" ";
        }
        cout<<endl;
    }*/
    for (int t = 1; t < length; t++)
    {
        vector<int> show(length, 0);
        for (int i = 0; i < NN.size(); i++)
        {
            if (NN[i].track == t)
            {
                for (int j = NN[i].point[0].x; j <= NN[i].point[NN[i].point.size() - 1].x; j++)
                {
                    show[j] = NN[i].n;
                }
            }
        }
        for (int i = 0; i < length; i++)
        {
            if (show[i] != 0)
                cout << show[i];
            else
                cout << " ";
            cout << " ";
        }
        cout << endl;
    }

    for (int i = 0; i < Bottom.size(); i++)
    {
        cout << Bottom[i] << " ";
    }
    cout << endl;

    /*for(int i = 1; i < length; i++){
        for(int j = 1; j < length; j++){
            if(graph[i][j] == 1){
                cout<<i<<"----->"<<j<<endl;
            }
        }
    }*/
}
void Print_Weight()
{
    for (int i = 0; i < NN.size(); i++)
    {
        if (NN[i].selected == false)
        {
            cout << NN[i].n << " : " << NN[i].weight << endl;
        }
    }
}

void ReadInput(string filename)
{
    ifstream infile;
    infile.open(filename, ios::in);

    if (!infile)
    {
        cout << "Cannot Open File " << filename << endl;
        exit(1);
    }

    string line1, line2;
    getline(infile, line1);
    getline(infile, line2);
    stringstream ss1(line1);
    stringstream ss2(line2);

    int top, bot, count = 0;
    while (ss1 >> top)
    {
        ss2 >> bot;
        cols.push_back(col{top, bot});
        if (top != 0)
        {
            nets[top].ll = (nets[top].ll == -1) ? count : nets[top].ll;
            nets[top].rr = max(nets[top].rr, count);
            nets_set.insert(top);
            MaxNum = max(top, MaxNum);
        }
        if (bot != 0)
        {
            nets[bot].ll = (nets[bot].ll == -1) ? count : nets[bot].ll;
            nets[bot].rr = max(nets[bot].rr, count);
            nets_set.insert(bot);
            MaxNum = max(bot, MaxNum);
        }

        count++;
    }

    Length = cols.size();
    vector<int> overlap(Length, 0);
    for (map<int, net>::iterator it = nets.begin(); it != nets.end(); it++)
    {

        if (it->second.ll != -1 && it->second.rr != -1)
        {
            for (int i = it->second.ll; i <= it->second.rr; i++)
            {
                overlap[i]++;
            }
            it->second.valid = true;
        }

        // cout << it->first << ", " << it->second.ll << ", " << it->second.rr << " : " << it->second.valid << endl;
    }

    for (int i = 0; i < Length; i++)
    {
        // cout << i << " :" << overlap[i] << endl;
        Density = max(Density, overlap[i]);
    }
    // cout << "Density: " << Density << endl;
    // Density = Density + 4;
}
void InitLUT()
{
    int m = 0;
    for (int i = 0; i < Length; i++)
    {
        m = max(cols[i].top, m);
        m = max(cols[i].bot, m);
    }

    LUT.clear();
    LUT.resize(m + 1);

    for (int c = 0; c < LUT.size(); c++)
    {
        LUT[c].resize(Length);
    }
    for (int c = 0; c < Length; c++)
    {
        for (int n = 0; n < LUT.size(); n++)
        {
            bool fall = false, raise = false;
            for (int i = c + 1; i < Length and i <= c + SNC; i++)
            {
                if (cols[i].top == n)
                {
                    raise = true;
                }
                if (cols[i].bot == n)
                {
                    fall = true;
                }
            }
            if (raise and fall)
                LUT[n][c] = 0;
            else if (raise)
                LUT[n][c] = 1;
            else if (fall)
                LUT[n][c] = -1;
            else
                LUT[n][c] = 0;
        }
    }
}
void GreedyRoute()
{
    // ------------------- Run Different Initail-Channel-Width -------------------
    int min_track = INT_MAX, chosed_snc = 0;
    for (int snc = 0; snc < cols.size(); snc++)
    {
        // break;
        SNC = snc;
        int ntrack = Route(snc);
        if (ntrack < min_track)
        {
            min_track = ntrack;
            chosed_snc = snc;
        }
        // break;
    }
    // return;
    SNC = chosed_snc;
    // cout << "Choosed: " << chosed_snc << endl;
    int ntrack = Route(chosed_snc);
    // cout << "Track: " << ntrack << endl;
    WriteOutput();
}
void WriteOutput()
{
    ofstream outfile;
    outfile.open(filename);

    for (int i = 1; i < nets.size(); i++)
    {
        if (not nets[i].valid)
            continue;
        else
        {
            outfile << ".begin " << i << endl;
            for (int x = 0; x < res_ver.size(); x++)
            {
                for (int y = 0; y < res_ver[0].size(); y++)
                {
                    if (res_ver[x][y] == i)
                    {
                        outfile << ".V " << x << " " << y << " ";
                        while (y + 1 < res_ver[0].size() && res_ver[x][y + 1] == i)
                            y++;
                        outfile << y << endl;
                    }
                }
            }
            for (int y = 0; y < res_hor[0].size(); y++)
            {
                for (int x = 0; x < res_hor.size(); x++)
                {
                    if (res_hor[x][y] == i)
                    {
                        if (x + 1 < res_hor.size() && res_hor[x + 1][y] == i)
                        {
                            outfile << ".H " << x << " " << y + 1 << " ";
                            while (x + 1 < res_hor.size() && res_hor[x + 1][y] == i)
                                x++;
                            outfile << x << endl;
                        }
                    }
                }
            }
            outfile << ".end" << endl;
        }
    }
    // cout << "END" << endl;
}

int Route(int snc)
{
    res_hor.clear();
    res_ver.clear();
    InitLUT();
    // all_route_complete change
    for (int n = 0; n < Length or not Complete(); n++)
    {
        bool t_flag = false, b_flag = false;
        vector<int> rowtmp(rows.size(), 0);
        for (int i = 0; i < rows.size() and n != 0; i++)
        {
            rowtmp[i] = rows[i].next;
        }
        if (n != 0)
        {
            rows.clear();
            rows.resize(rowtmp.size());
        }
        else
        {
            rows.resize(Density);
        }
        for (int i = 0; i < rows.size(); i++)
        {
            if (n == 0)
                rows[i].hor = 0;
            else
                rows[i].hor = rowtmp[i];
            rows[i].next = 0;
            rows[i].tmp = 0;
            rows[i].ver = 0;
        }
        // cout << "==A==" << endl;
        // Print();
        //------------- Steps A -----------------
        A(n, cols[n].top, cols[n].bot, t_flag, b_flag);
        // cout << "==0==" << endl;
        // Print();
        //------------- Steps B -----------------
        B(n, cols[n].top, cols[n].bot);
        // cout << "==1==" << endl;
        // Print();
        // cout << "==1==" << endl;

        //------------- Steps C -----------------
        C(n);
        // cout << "==2==" << endl;
        // Print();
        //------------- Steps D -----------------
        D(n);

        //------------- Steps E -----------------
        E(n, cols[n].top, cols[n].bot, t_flag, b_flag);
        // cout << "==3==" << endl;
        // Print();

        //------------- Steps F -----------------
        F(n);
        // break;
        // cout << "==4==" << endl;
        // Print();
        // cout << "========" << endl;
    }

    //      << endl;
    return PostProcess();
}

void A(int c, int top, int bot, bool &t_f, bool &b_f)
{
    // case: top and bot are 0
    if (top == 0 and bot == 0)
    {
        t_f = true;
        b_f = true;
        return;
    }
    // case: same column signal without other upper pins
    // if (top == bot and top != 0 and bot != 0 and nets[top].ll == nets[top].rr)
    if (false)
    {
        for (int i = 0; i < rows.size(); i++)
        {
            rows[i].ver = top;
        }
        t_f = true;
        b_f = true;
        return;
    }
    else
    {
        // case: there are other bottom pins
        if (bot != 0)
        {
            int steps = 0;
            while (rows[steps].hor != 0 and rows[steps].hor != bot)
            {
                if (++steps < 0 or rows[steps].ver != 0)
                {
                    return;
                }
            }
            b_f = true;
            for (int i = 0; i <= steps; i++)
            {
                rows[i].ver = bot;
            }
            rows[steps].hor = bot;
        }
        if (top != 0)
        {
            int steps = rows.size() - 1;
            while (rows[steps].hor != 0 and rows[steps].hor != top)
            {
                if (--steps < 0 or rows[steps].ver != 0)
                {
                    return;
                }
            }
            for (int i = rows.size() - 1; i >= steps; i--)
            {
                rows[i].ver = top;
            }

            rows[steps].hor = top;
            t_f = true;
        }
    }
}
void E(int c, int top, int bot, bool t_f, bool b_f)
{
    if (not b_f and bot != 0)
    {
        int tmp_row = 0;
        while (rows[tmp_row].ver == 0 && tmp_row < rows.size() / 2)
            tmp_row++;
        for (int i = 0; i < tmp_row; i++)
        {
            rows[i].ver = bot;
        }
        row tmpr{bot, bot, bot, bot};
        tmpr.tmp = 0;
        rows.insert(rows.begin() + tmp_row, tmpr);
        // cout << "Insert2: " << tmp_row << ", " << bot << endl;
        int y = tmp_row + 1;
        for (int i = 0; i < res_hor.size(); i++)
        {
            vector<int> &tmp = res_hor[i];
            tmp.insert(tmp.begin() + y, 0);
        }
        for (int i = 0; i < res_ver.size(); i++)
        {
            vector<int> &tmp = res_ver[i];
            if (y == tmp.size())
                tmp.insert(tmp.begin() + y, tmp[y - 1]);
            else if (y == 0)
                tmp.insert(tmp.begin() + y, tmp[0]);
            else if (y > 0 && tmp[y] == tmp[y - 1])
                tmp.insert(tmp.begin() + y, tmp[y]);
            else
                tmp.insert(tmp.begin() + y, 0);
        }
    }
    if (not t_f and top != 0)
    {
        int tmp_row = rows.size() - 1;
        while (rows[tmp_row].ver == 0 && tmp_row > rows.size() / 2)
            tmp_row--;
        for (int i = rows.size() - 1; i > tmp_row; i--)
        {
            rows[i].ver = top;
        }
        row tmpr{top, top, top, top};
        tmpr.tmp = 0;
        rows.insert(rows.begin() + tmp_row + 1, tmpr);
        // cout << "Insert1: " << tmp_row << ", " << top << endl;

        int y = tmp_row + 1;
        for (int i = 0; i < res_hor.size(); i++)
        {
            vector<int> &tmp = res_hor[i];
            tmp.insert(tmp.begin() + y, 0);
        }
        for (int i = 0; i < res_ver.size(); i++)
        {
            vector<int> &tmp = res_ver[i];
            if (y == tmp.size())
                tmp.insert(tmp.begin() + y, tmp[y - 1]);
            else if (y == 0)
                tmp.insert(tmp.begin() + y, tmp[0]);
            else if (y > 0 && tmp[y] == tmp[y - 1])
                tmp.insert(tmp.begin() + y, tmp[y]);
            else
                tmp.insert(tmp.begin() + y, 0);
        }
    }
}
void B(int c, int top, int bot)
{
    for (int i = 0; i < rows.size(); i++)
    {
        if (rows[i].hor != 0)
        {
            int collapse_net = rows[i].hor;
            int tmp_row = i + 1;
            while (tmp_row < rows.size() and rows[tmp_row].hor != collapse_net)
            {
                if (rows[tmp_row].ver != 0 and rows[tmp_row].ver != collapse_net)
                    tmp_row = rows.size();
                tmp_row += 1;
            }
            if (tmp_row < rows.size())
            {
                for (int j = i; j <= tmp_row; j++)
                {
                    rows[j].ver = collapse_net;
                }
                if (LUT[collapse_net][c] == 1)
                {
                    rows[i].tmp = collapse_net;
                    rows[i].hor = 0;
                }
                else
                {
                    rows[tmp_row].tmp = collapse_net;
                    rows[tmp_row].hor = 0;
                }
            }
        }
    }
}
void D(int c)
{
    for (int i = 0; i < rows.size(); i++)
    {
        if (rows[i].tmp != 0)
            rows[i].hor = rows[i].tmp;
    }
}
void C(int c)
{
    for (int y1 = 0, y2 = rows.size() - 1; y1 <= y2; y1++, y2--)
    {
        if (y1 != y2)
        {
            if (rows[y1].hor != 0 and not Finished(c, rows[y1].hor))
            {
                // cout << "LUT:" << y1 << ", " << LUT[rows[y1].hor][c] << endl;
                if (LUT[rows[y1].hor][c] == 1)
                {
                    int sig = rows[y1].hor;
                    int tmp_row = y1;
                    while (tmp_row < rows.size())
                    {
                        if (rows[++tmp_row].ver != 0)
                        {
                            break;
                        }
                    }
                    while (tmp_row > y1)
                    {
                        if (rows[--tmp_row].hor == 0)
                        {
                            break;
                        }
                    }
                    if (tmp_row - y1 >= Min_Jog)
                    {
                        for (int r = y1; r <= tmp_row; r++)
                        {
                            rows[r].ver = sig;
                        }
                        rows[tmp_row].hor = sig;
                        rows[tmp_row].next = sig;
                    }
                    else
                    {
                        rows[y1].next = sig;
                    }
                    // cout << "1:" << tmp_row << ", " << y1 << ", " << Min_Jog << endl;
                }
                else if (LUT[rows[y1].hor][c] == -1)
                {
                    int sig = rows[y1].hor;
                    int tmp_row = y1;
                    while (tmp_row >= 0)
                    {
                        if (rows[--tmp_row].ver != 0)
                        {
                            break;
                        }
                    }
                    while (tmp_row < y1)
                    {
                        if (rows[++tmp_row].hor == 0)
                        {
                            break;
                        }
                    }
                    if (y1 - tmp_row >= Min_Jog)
                    {
                        for (int r = y1; r >= tmp_row; r--)
                        {
                            rows[r].ver = sig;
                        }
                        rows[tmp_row].hor = sig;
                        rows[tmp_row].next = sig;
                    }
                    else
                    {
                        rows[y1].next = sig;
                    }
                    // cout << "-1:" << tmp_row << ", " << y1 << ", " << Min_Jog << endl;
                }
                else
                {
                    // Change
                    rows[y1].next = rows[y1].hor;
                }
            }

            if (rows[y2].hor != 0 and not Finished(c, rows[y2].hor))
            {
                // cout << "LUT:" << y2 << ", " << LUT[rows[y2].hor][c] << endl;
                if (LUT[rows[y2].hor][c] == 1)
                {

                    int sig = rows[y2].hor;
                    int tmp_row = y2;
                    while (tmp_row < rows.size())
                    {
                        if (rows[++tmp_row].ver != 0)
                        {
                            break;
                        }
                    }
                    while (tmp_row > y2)
                    {
                        if (rows[--tmp_row].hor == 0)
                        {
                            break;
                        }
                    }
                    if (tmp_row - y2 >= Min_Jog)
                    {
                        for (int r = y2; r <= tmp_row; r++)
                        {
                            rows[r].ver = sig;
                        }
                        rows[tmp_row].hor = sig;
                        rows[tmp_row].next = sig;
                    }
                    else
                    {
                        rows[y2].next = sig;
                    }
                }
                else if (LUT[rows[y2].hor][c] == -1)
                {
                    int sig = rows[y2].hor;
                    int tmp_row = y2;
                    while (tmp_row >= 0)
                    {
                        if (rows[--tmp_row].ver != 0)
                        {
                            break;
                        }
                    }
                    while (tmp_row < y2)
                    {
                        if (rows[++tmp_row].hor == 0)
                        {
                            break;
                        }
                    }
                    if (y2 - tmp_row >= Min_Jog)
                    {
                        for (int r = y2; r >= tmp_row; r--)
                        {
                            rows[r].ver = sig;
                        }
                        rows[tmp_row].hor = sig;
                        rows[tmp_row].next = sig;
                    }
                    else
                    {
                        rows[y2].next = sig;
                    }
                }
                else
                {
                    // Change
                    rows[y2].next = rows[y2].hor;
                    // cout << "0:" << y2 << ", " << Min_Jog << endl;
                }
            }
        }
        else
        {
            if (rows[y1].hor != 0 and not Finished(c, rows[y1].hor))
            {
                if (LUT[rows[y1].hor][c] == 1)
                {
                    int sig = rows[y1].hor;
                    int tmp_row = y1;
                    while (tmp_row < rows.size())
                    {
                        if (rows[++tmp_row].ver != 0)
                        {
                            break;
                        }
                    }
                    while (tmp_row > y1)
                    {
                        if (rows[--tmp_row].hor == 0)
                        {
                            break;
                        }
                    }
                    if (tmp_row - y1 >= Min_Jog)
                    {
                        for (int r = y1; r <= tmp_row; r++)
                        {
                            rows[r].ver = sig;
                        }
                        rows[tmp_row].hor = sig;
                        rows[tmp_row].next = sig;
                    }
                    else
                    {
                        rows[y1].next = sig;
                    }
                }
                else if (LUT[rows[y1].hor][c] == -1)
                {
                    int sig = rows[y1].hor;
                    int tmp_row = y1;
                    while (tmp_row >= 0)
                    {
                        if (rows[--tmp_row].ver != 0)
                        {
                            break;
                        }
                    }
                    while (tmp_row < y1)
                    {
                        if (rows[++tmp_row].hor == 0)
                        {
                            break;
                        }
                    }
                    if (y1 - tmp_row >= Min_Jog)
                    {
                        for (int r = y1; r >= tmp_row; r--)
                        {
                            rows[r].ver = sig;
                        }
                        rows[tmp_row].hor = sig;
                        rows[tmp_row].next = sig;
                    }
                    else
                    {
                        rows[y1].next = sig;
                    }
                }
                else
                {
                    // Change
                    rows[y1].next = rows[y1].hor;
                }
            }
        }
        // cout << rows[y1].hor << ":" << y1 << ", " << rows[y2].hor << ":" << y2 << endl;
        // Print();
    }
}
void F(int c)
{
    for (int i = 0; i < rows.size(); i++)
    {
        bool f1 = (rows[i].next == 0);
        bool f2 = (nets[rows[i].next].rr > c);
        int ntrack = 0;
        for (int r = 0; r < rows.size(); r++)
        {
            if (rows[i].next == rows[r].next)
                ntrack += 1;
        }
        bool f3 = (ntrack > 1);
        if (not f1 and not f2 and not f3)
            rows[i].next = 0;
    }

    vector<int> res1, res2;
    for (int i = 0; i < rows.size(); i++)
    {
        res1.push_back(rows[i].hor);
        res2.push_back(rows[i].ver);
    }
    res_hor.push_back(res1);
    res_ver.push_back(res2);
}
bool Finished(int c, int sig)
{
    if (nets[sig].rr >= c)
        return false;
    int ntrack = 0;
    for (int i = 0; i < rows.size(); i++)
    {
        if (sig == rows[i].hor)
            ntrack += 1;
    }
    if (ntrack > 1)
        return false;

    return true;
}
int PostProcess()
{
    for (int x = 0; x < res_ver.size(); x++)
    {
        vector<int> &c = res_ver[x];
        if (x < cols.size())
        {
            if (cols[x].bot != 0)
                c.insert(c.begin(), cols[x].bot);
            else
                c.insert(c.begin(), 0);
            if (cols[x].top != 0)
                c.push_back(cols[x].top);
            else
                c.push_back(0);
        }
        else
        {
            c.insert(c.begin(), 0);
            c.push_back(0);
        }
    }
    return res_ver[0].size() - 2;
}
bool Complete()
{
    for (int i = 0; i < rows.size(); i++)
        if (rows[i].next != 0)
            return false;
    return true;
}
void Print()
{
    for (int i = 0; i < rows.size(); i++)
    {
        cout << rows[i].hor << ", " << rows[i].ver << ", " << rows[i].next << endl;
    }
}
