#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include <random>
#include <chrono>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <iterator>

using namespace std;

// ***** Struct Definition *****

struct P
{
    int x;
    int y;
};
struct Macro
{
    int w;
    int h;
    P p;
};

// ***** Function Prototype *****
void Floorplan(double ratio, string f_block, string f_net, string f_utpur);
void Read_Net(string filename);
void Read_Block(string filename);
void Write_Output(string filename);
void PrintAll();
void PrintBlock();
void SA_Init();
void SA_FP();
void OP1(int b1, int b2);
void OP2(int b1, int b2, int choice);
void OP3(int b1, int b2);

int Get_HPWL();
int Get_Area();
double Get_Cost();

// ***** Variables *****
double H = 0, W = 0;
double calcul_r = 0, golden_r, alpha = 0;
int min_cost = std::numeric_limits<int>::max();
int N_Block = 0, N_Terminal = 0, N_net = 0, N_candidate = 1;
int b_H, b_W, timelimit = 298, wire_len = 0;

map<string, Macro>
    Macros;
map<string, P> Terminals;
vector<set<string> > Nets;
vector<string> MacroStr;
vector<pair<int, map<string, Macro> > > candidate;
vector<vector<int> > cand_Pos;
vector<vector<int> > cand_Neg;

vector<int> Pos_Loci, Neg_Loci;
vector<int> Block2Pos_Loci, Block2Neg_Loci;
vector<P> res;
time_t t_1, t_2;

random_device rnd;
default_random_engine eng(rnd());

class Weight
{
    std::map<char, double> w_map;
    const double de_weight;
    const double penal;

public:
    Weight(const double &de_weight, const double &pen) : de_weight(de_weight), penal(pen){};
    void Set(const char &letter, const double &weight) { w_map[letter] = weight; }
    const double Get(const char &letter)
    {
        if (w_map.find(letter) != w_map.end())
        {
            return w_map[letter] - penal;
        }
        else
        {
            return de_weight - penal;
        }
    }
};
class LCS
{
    const std::string str1, str2;

public:
    LCS(const std::string &s1, const std::string &s2)
        : str1(s1), str2(s2){};
    std::pair<std::string, double> GetMax(Weight &w_map)
    {
        // MaxSubstring res;

        for (int i = 0; i < str1.size(); ++i)
        {
            // res.curr = MaxDiagSum(weights, i, 0);
            // res.CheckMax();
        }
        for (int j = 1; j < str2.size(); ++j)
        {
            // res.curr = MaxDiagSum(weights, 0, j);
            // res.CheckMax();
        }

        // if (res.max.Empty())
        // {
        //     return std::make_pair("", 0);
        // }
        return std::make_pair(str1, 0.000);
    }
    // Substring MaxDiagSum(LetterWeights &weights, const int i_start, const int j_start)
    // {
    //     MaxSubstring res;
    //     int i1 = i_start, i2 = j_start;
    //     for (; i1 < string1.size() && i2 < string2.size(); ++i1, ++i2)
    //     {
    //         if (string1[i1] == string2[i2])
    //         {
    //             res.Add(i1, weights.Get(string1[i1]));
    //         }
    //         else
    //         {
    //             res.CheckMax();
    //         }
    //     }
    //     res.CheckMax();
    //     return res.max;
    // }
};

// ***** Function Definition *****
void Floorplan(double alp, string f_block, string f_net, string f_output)
{
    t_1 = time(NULL);
    alpha = alp;
    Read_Block(f_block);
    Read_Net(f_net);
    do
    {
        SA_Init();
        SA_FP();
        t_2 = time(NULL);
        // cout << "Next" << endl;

    } while (difftime(t_2, t_1) < timelimit);

    Write_Output(f_output);
}
void Write_Output(string filename)
{
    ofstream outfile(filename);
    outfile.precision(numeric_limits<double>::max_digits10 + 1);
    int index = 0;
    if (candidate.size() != 0)
    {
        for (int i = 0; i < candidate.size(); i++)
        {
            if (candidate[i].first < candidate[index].first)
            {
                index = i;
            }
        }
        Macros = candidate[index].second;
        Pos_Loci = cand_Pos[index];
        Neg_Loci = cand_Neg[index];
        for (int j = 0; j < N_Block; j++)
        {
            Block2Pos_Loci[Pos_Loci[j]] = j;
            Block2Neg_Loci[Neg_Loci[j]] = j;
        }
    }

    int area = Get_Area();
    int wire = Get_HPWL();
    double cost = (double)alpha * area + (double)(1 - alpha) * wire;
    t_2 = time(NULL);

    outfile << cost << endl
            << wire << endl
            << area << endl
            << b_W << " " << b_H << endl
            << difftime(t_2, t_1) << endl;

    for (map<string, Macro>::iterator it = Macros.begin(); it != Macros.end(); it++)
    {
        outfile << it->first << " " << it->second.p.x << " " << it->second.p.y << " " << (it->second.p.x + it->second.w) << " " << (it->second.p.y + it->second.h) << endl;
    }
}
void SA_Init()
{

    shuffle(Pos_Loci.begin() + 1, Pos_Loci.end(), default_random_engine(chrono::system_clock::now().time_since_epoch().count()));
    shuffle(Neg_Loci.begin() + 1, Neg_Loci.end(), default_random_engine(chrono::system_clock::now().time_since_epoch().count()));

    for (int i = 0; i < N_Block; i++)
    {
        Block2Pos_Loci[Pos_Loci[i]] = i;
        Block2Neg_Loci[Neg_Loci[i]] = i;
    }

    // cout << "======== Initialization ========" << endl;
    // PrintBlock();
}
void Read_Block(string filename)
{
    ifstream infile(filename);
    string white;

    infile >> white >> W >> H >> white >> N_Block >> white >> N_Terminal;

    MacroStr.resize(N_Block);
    Pos_Loci.resize(N_Block);
    Neg_Loci.resize(N_Block);
    Block2Neg_Loci.resize(N_Block);
    Block2Pos_Loci.resize(N_Block);

    // cout << "W: " << W << " H: " << H << " Blocks: " << N_Block << " Terminals: " << N_Terminal << endl;
    if (W > H)
    {
        golden_r = H / W;
    }
    else
    {
        golden_r = W / H;
    }
    string tmpstr;
    int tmpw, tmpl;
    P p{0, 0};

    for (int i = 0; i < N_Block; i++)
    {
        infile >> tmpstr >> tmpw >> tmpl;
        Macros[tmpstr] = Macro{tmpw, tmpl, p};
        Pos_Loci[i] = i;
        Neg_Loci[i] = i;
        MacroStr[i] = tmpstr;
        // cout << tmpstr << " " << tmpw << " " << tmpl << endl;
    }
    // infile >> white;
    for (int i = 0; i < N_Terminal; i++)
    {
        infile >> tmpstr >> white >> tmpw >> tmpl;
        p = P{tmpw, tmpl};
        Terminals[tmpstr] = P{p};
        // cout << tmpstr << " " << tmpw << " " << tmpl << endl;
    }
    infile.close();
}
void Read_Net(string filename)
{
    ifstream infile(filename);
    string white;

    infile >> white >> N_net;
    int degree;
    string str;
    for (int i = 0; i < N_net; i++)
    {
        infile >> white >> degree;
        set<string> tmp;
        for (int j = 0; j < degree; j++)
        {
            infile >> str;
            tmp.insert(str);
        }
        Nets.push_back(tmp);
    }
    infile.close();
}
//
void SA_FP() // Simulated Annealing for Floorplanning
{
    uniform_int_distribution<> choice(1, 3);
    uniform_int_distribution<> cha_bl(0, N_Block - 1);
    uniform_real_distribution<> exp_eng(0.0, 1.0);
    uniform_int_distribution<> xy(1, 2);

    double T = 10; // Initial temperature

    // SA Parameter
    int numIterations = 1200;
    int fail = 0;
    int R_factor = 10;
    bool stop = false;

    double Tmin = .00000001; // Temperature at which iteration terminates
    double beta = 0.7;       // Decrease in temperature
    int cost = Get_Cost() + R_factor * abs(golden_r - calcul_r);

    t_2 = difftime(time(NULL), t_1);

    while (T > Tmin && t_2 < timelimit && !stop)
    {
        // cout << "SA Fail: " << fail << endl;
        fail = 0;
        stop = false;
        for (int i = 0; i < numIterations; i++)
        {
            // cout << "i: " << i;
            // PrintBlock();
            int chosed = choice(eng);
            int block1 = cha_bl(eng);
            int block2 = cha_bl(eng);
            int choice = xy(eng);

            // cout << " before switch ";
            switch (chosed)
            {
            case 1:
                OP1(block1, block2);
                break;
            case 2:
                OP2(block1, block2, choice);
                break;
            case 3:
                OP3(block1, block2);
                break;
            }
            // cout << "======== Switch End1 ========" << endl;
            int real_cost = Get_Cost();
            int curr_cost = real_cost;

            if (b_W > W || b_H > H)
            {
                curr_cost += R_factor * (abs(b_H - H) + abs(b_W - W));
            }

            // cout << "======== Get Cost1 ========" << endl;
            // cout << " fail " << (curr_cost > cost) << (b_H > H) << (b_W > W);
            if (curr_cost > cost)
            {
                double myexp = exp((cost - curr_cost) / T);
                if (exp_eng(eng) > myexp)
                {
                    fail += 1;
                    switch (chosed)
                    {
                    case 1:
                        OP1(block1, block2);
                        break;
                    case 2:
                        OP2(block1, block2, choice);
                        break;
                    case 3:
                        OP3(block1, block2);
                        break;
                    }
                    continue;
                }
            }
            cost = curr_cost;
            if ((b_W < W && b_H < H))
            {
                if (real_cost > min_cost)
                {
                    // cout << "continue" << endl;
                    continue;
                }
                // cout << "min_cost: " << min_cost;
                min_cost = real_cost;
                if (candidate.size() < N_candidate)
                {

                    candidate.push_back(pair<int, map<string, Macro> >(real_cost, Macros));
                    cand_Pos.push_back(Pos_Loci);
                    cand_Neg.push_back(Neg_Loci);
                    // cout << real_cost << " push back: " << b_W << ", " << b_H << endl;
                }
                else
                {
                    for (int i = 0; i < N_candidate; i++)
                    {
                        if (candidate[i].first > real_cost)
                        {
                            candidate[i] = pair<int, map<string, Macro> >(real_cost, Macros);
                            cand_Neg[i] = Neg_Loci;
                            cand_Pos[i] = Pos_Loci;

                            break;
                        }
                    }
                }
                // cout << "Pass" << endl;
            }
            if ((double)(fail * 1.0 / numIterations) > 0.96)
            {
                stop = true;
                // cout << "fail" << endl;
            }
        }
        // PrintBlock();

        T *= beta;
        t_2 = difftime(time(NULL), t_1);
    }
    // cout << "Iteration" << endl;
    // cout << "[END SA]" << endl;
}
int Get_HPWL()
{
    // cout << "[Start HPWL]";
    int res = 0;
    for (int i = 0; i < Nets.size(); i++)
    {
        int ll = numeric_limits<int>::max(); // left
        int bb = numeric_limits<int>::max(); // bottom
        int rr = 0;                          // right
        int tt = 0;                          // top

        set<string> tmp = Nets[i];
        for (set<string>::iterator it = Nets[i].begin(); it != Nets[i].end(); it++)
        {
            if (Macros.find(*it) != Macros.end())
            {
                Macro m = Macros.find(*it)->second;
                // cout << *it << " in Macros" << endl;
                int tmpX = m.p.x + m.w / 2;
                int tmpY = m.p.y + m.h / 2;

                ll = min(ll, tmpX);
                rr = max(rr, tmpX);
                bb = min(bb, tmpY);
                tt = max(tt, tmpY);
            }
            else
            {
                P p = Terminals.find(*it)->second;
                int tmpX = p.x;
                int tmpY = p.y;

                ll = min(ll, tmpX);
                rr = max(rr, tmpX);
                bb = min(bb, tmpY);
                tt = max(tt, tmpY);
            }
        }
        res += rr - ll + tt - bb;
    }
    // cout << "[End HWPL]";
    wire_len = res;
    return res;
}
int Get_Area()
{
    map<int, int> L;           // tmp Length
    vector<int> CorX(N_Block); // Coordinate of X
    vector<int> CorY(N_Block);
    L[0] = 0;
    for (int i = 0; i < N_Block; i++)
    {
        int block = Pos_Loci[i];
        int Pos_Neg = Block2Neg_Loci[block];
        map<int, int>::iterator ll = L.lower_bound(Pos_Neg);
        if (ll != L.begin())
        {
            ll--;
        }
        CorX[block] = ll->second;
        int tmpX = CorX[block] + Macros[MacroStr[block]].w;
        L[Pos_Neg] = tmpX;
        map<int, int>::iterator tt = L.upper_bound(Pos_Neg);
        while (tt != L.end())
        {
            if (tt->second < tmpX)
            {
                tt = L.erase(tt);
            }
            else
                tt++;
        }

        Macros[MacroStr[block]].p.x = CorX[block];
    }
    b_W = (prev(L.end()))->second;
    L.clear();
    L[0] = 0;
    for (int i = N_Block - 1; i >= 0; i--)
    {
        int block = Pos_Loci[i];
        int Pos_Neg = Block2Neg_Loci[block];
        map<int, int>::iterator ll = L.lower_bound(Pos_Neg);
        if (ll != L.begin())
        {
            ll--;
        }
        CorY[block] = ll->second;
        int tmpY = CorY[block] + Macros[MacroStr[block]].h;
        L[Pos_Neg] = tmpY;
        map<int, int>::iterator tt = L.upper_bound(Pos_Neg);
        while (tt != L.end())
        {
            if (tt->second < tmpY)
            {
                tt = L.erase(tt);
            }
            else
                tt++;
        }
        Macros[MacroStr[block]].p.y = CorY[block];
    }
    b_H = (prev(L.end()))->second;
    // cout << "[End Area]";
    return b_W * b_H;
}
double Get_Cost()
{
    // cout << "[Start Cost]";
    int area = Get_Area();
    int wire = Get_HPWL();
    // cout << "Area: " << area << " Wire: " << wire << endl;

    if (H > W)
    {
        calcul_r = (double)b_W / b_H;
    }
    else
    {
        calcul_r = (double)b_H / b_W;
    }
    // cout << "[End Cost]";
    return alpha * area + (1 - alpha) * wire;
}
void OP1(int b1, int b2)
{

    string str = MacroStr[b1];

    // cout << "======== OP1 ========: " << str << endl;
    int t = Macros[str].h;
    Macros[str].h = Macros[str].w;
    Macros[str].w = t;
}
void OP2(int b1, int b2, int choice)
{

    if (choice == 1)
    {
        int id1 = Block2Pos_Loci[b1];
        int id2 = Block2Pos_Loci[b2];

        int loci1 = Pos_Loci[id1];
        int loci2 = Pos_Loci[id2];

        // cout << "======== OP2 ========: " << choice << ": " << MacroStr[loci1] << ", " << MacroStr[loci2] << endl;

        Pos_Loci[id2] = loci1;
        Pos_Loci[id1] = loci2;

        Block2Pos_Loci[b1] = id2;
        Block2Pos_Loci[b2] = id1;
    }
    else
    {
        int id1 = Block2Neg_Loci[b1];
        int id2 = Block2Neg_Loci[b2];

        int loci1 = Neg_Loci[id1];
        int loci2 = Neg_Loci[id2];

        // cout << "======== OP2 ========: " << choice << ": " << MacroStr[loci1] << ", " << MacroStr[loci2] << endl;

        Neg_Loci[id1] = loci2;
        Neg_Loci[id2] = loci1;

        Block2Neg_Loci[b1] = id2;
        Block2Neg_Loci[b2] = id1;
    }
}
void OP3(int b1, int b2)
{

    int id1 = Block2Pos_Loci[b1];
    int id2 = Block2Pos_Loci[b2];

    int loci1 = Pos_Loci[id1];
    int loci2 = Pos_Loci[id2];

    // cout << "======== OP3 ======== : " << MacroStr[loci1] << ", " << MacroStr[loci2] << endl;

    Pos_Loci[id2] = loci1;
    Pos_Loci[id1] = loci2;

    Block2Pos_Loci[b1] = id2;
    Block2Pos_Loci[b2] = id1;

    id1 = Block2Neg_Loci[b1];
    id2 = Block2Neg_Loci[b2];

    loci1 = Neg_Loci[id1];
    loci2 = Neg_Loci[id2];

    Neg_Loci[id1] = loci2;
    Neg_Loci[id2] = loci1;

    Block2Neg_Loci[b1] = id2;
    Block2Neg_Loci[b2] = id1;
}
void PrintAll()
{
    cout << "======== Macro ========" << endl;
    for (map<string, Macro>::iterator it = Macros.begin(); it != Macros.end(); it++)
    {
        cout << it->first << "|"
             << "\t" << it->second.h << "\t" << it->second.w << "\t"
             << "( " << it->second.p.x << ", " << it->second.p.y << " )" << endl;
    }

    cout << "======== Terminal ========" << endl;
    for (map<string, P>::iterator it = Terminals.begin(); it != Terminals.end(); it++)
    {
        cout << it->first << "|"
             << "\t" << it->second.x << "\t" << it->second.y << endl;
    }

    cout << "======== Net ========" << endl;
    for (int i = 0; i < Nets.size(); i++)
    {
        cout << " (" << Nets[i].size() << ") ";
        for (set<string>::iterator j = Nets[i].begin(); j != Nets[i].end(); j++)
        {
            cout << *j << "\t";
        }
        cout << endl;
    }
}
void PrintBlock()
{
    int area = Get_Area();
    int wire = Get_HPWL();
    double cost = (double)alpha * area + (double)(1 - alpha) * wire;
    t_2 = time(NULL);

    cout << cost << endl
         << wire << endl
         << area << endl
         << b_W << " " << b_H << endl
         << difftime(t_2, t_1) << endl;
    for (map<string, Macro>::iterator it = Macros.begin(); it != Macros.end(); it++)
    {
        cout << it->first << " " << it->second.p.x << " " << it->second.p.y << " " << (it->second.p.x + it->second.w) << " " << (it->second.p.y + it->second.h) << endl;
    }

    cout << "===== Postive Loci =====" << endl;
    for (int i = 0; i < N_Block; i++)
    {
        cout << MacroStr[Pos_Loci[i]] << " ";
    }
    cout << endl;
    cout << "===== Negative Loci =====" << endl;
    for (int i = 0; i < N_Block; i++)
    {
        cout << MacroStr[Neg_Loci[i]] << " ";
    }
    cout << endl;
}
