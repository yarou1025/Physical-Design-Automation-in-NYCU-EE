#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

struct Cell
{
    string str;
    int x1, y1;
    int x2, y2; // Legal
    int w, h;
};

struct Terminal // Fixed and Cannot be overlaped
{
    int x1, y1;
    int w, h;
};

struct Cluster
{
    Cluster()
    {
        Weight = 0;
        Width = 0;
        Quadratic = 0;
    };
    int chosed_x, Weight, Width, Quadratic, Firstcell, Lastcell;
};

struct Row
{
    Row()
    {
        Height = 0;
        Sitespacing = 1;
        SubrowOrigin = 0;
        NumSites = 0;
        Width = 0;
    };
    int Coordinate, Height, Sitespacing, SubrowOrigin, NumSites, Width, Right;
    vector<Cluster> clusters;
    vector<int> nodes;
};

string f_path, f_node, f_pl, f_scl;
int nRows, nNodes, nTerminals;
int nTotalWidth, nMinWidth, nMaxHeight;
vector<Cell> cells;
map<string, int> cell2int;
map<string, Terminal> terminals;
vector<Row> rows;
map<int, set<int> > sameHeight;
time_t t_1, t_2;
int timelimit = 300;

bool cmp_x(Cell &c1, Cell &c2);
bool cmp_y(Row &r1, Row &r2);

void Collapse(Cluster &cluster, int left, int right, int tmp, vector<Cluster> &clusters);
void PlaceRow(Row &r);
void Parser(string f_aux);
void Abacus();
void PrintCell(Cell c);

int main(int argc, char *argv[])
{
    t_1 = time(NULL);

    if (argc < 2)
    {
        cout << "No aux file" << endl;
        exit(1);
    }

    string f_aux = argv[1];
    Parser(f_aux);
    sort(rows.begin(), rows.end(), cmp_y);
    sort(cells.begin(), cells.end(), cmp_x);
    Abacus();

    cout << "After Abacus" << endl;
    for (int i = 0; i < cells.size(); ++i)
    {
        cell2int[cells[i].str] = i;
    }

    ifstream infile(f_pl);
    ofstream outfile("output.pl");
    string line;
    getline(infile, line);
    outfile << line << endl;
    while (getline(infile, line))
    {
        if (line.find("#") == 0 || line.empty())
        {
            outfile << line << endl;
            continue;
        }
        stringstream ss(line);
        string str;

        ss >> str;
        if (line.find("FIXED_NI") != string::npos)
            outfile << line << endl;
        else if (line.find("FIXED") != string::npos)
            outfile << line << endl;
        else
        {
            outfile << str << " " << cells[cell2int[str]].x2 << " " << cells[cell2int[str]].y2 << " : N" << endl;
        }
    }

    cout << difftime(time(NULL), t_1) << endl;
    infile.close();
    outfile.close();

    return 0;
}

bool cmp_y(Row &r1, Row &r2)
{
    if (r1.Coordinate == r2.Coordinate)
        return r1.SubrowOrigin < r2.SubrowOrigin;
    else
        return r1.Coordinate < r2.Coordinate;
}
bool cmp_x(Cell &c1, Cell &c2)
{
    return c1.x1 < c2.x1;
}
void Parser(string f_aux)
{
    ifstream infile(f_aux);
    string strTmp, strPath = "", line = "";
    string strNode, strNet, strWts, strPl, strScl, strShape, strRoute;

    infile >> strTmp >> strTmp >> strNode >> strNet >> strWts >> strPl >> strScl >> strShape >> strRoute;

    infile.close();

    vector<string> src_path;
    size_t pos = 0;
    while ((pos = f_aux.find("/")) != string::npos)
    {
        src_path.push_back(f_aux.substr(0, pos));
        f_aux.erase(0, pos + 1);
    }

    f_path = "";
    for (int i = 0; i < src_path.size(); i++)
    {
        f_path += src_path[i] + "/";
    }

    f_node = f_path + strNode;
    f_pl = f_path + strPl;
    f_scl = f_path + strScl;

    // ========= Parse Nodes ========
    infile.open(f_node);
    do
    {
        getline(infile, line);
    } while (line.find("NumNodes") == string::npos);

    stringstream ss(line);
    ss >> strTmp >> strTmp >> nNodes;

    getline(infile, line);
    ss.clear();
    ss.str(line);
    ss >> strTmp >> strTmp >> nTerminals;

    while (getline(infile, line))
    {
        if (line.empty() || line.find("#") == 0)
            continue;
        stringstream ss(line);
        string str;
        int w, h;
        ss >> str >> w >> h;
        // cout << "Line" << line << endl;
        if (line.find("terminal_NI") != string::npos)
            continue;
        else if (line.find("terminal") != string::npos)
        {
            terminals[str] = Terminal{0, 0, w, h};
        }
        else
        {
            cells.push_back({str, 0, 0, 0, 0, w, h});
            cell2int[str] = cells.size() - 1;
        }
        line.clear();
    }
    infile.close();

    // ========= Parse PL ========
    infile.open(f_pl);

    while (getline(infile, line))
    {
        if (line.empty() || line.find("#") == 0)
            continue;
        stringstream ss(line);
        string str;
        int x1, y1;
        ss >> str >> x1 >> y1;
        // cout << "Line" << line << endl;
        if (line.find("FIXED_NI") != string::npos)
            continue;
        else if (line.find("FIXED") != string::npos)
        {
            terminals[str].x1 = x1;
            terminals[str].y1 = y1;
        }
        else
        {
            int id = cell2int[str];
            cells[id].x1 = x1;
            cells[id].y1 = y1;
        }
        line.clear();
    }
    infile.close();

    // cout << cells["o169171"].x1 << ", " << cells["o169171"].y1 << ", " << cells["o169171"].w << ", " << cells["o169171"].h << endl;

    // ========= Parse SCL ========
    int nTotalWidth = 0, nMinWidth = INT_MAX;
    int nMaxHeight = INT_MIN;
    int nMinHeight = INT_MAX;
    infile.open(f_scl);
    do
    {
        getline(infile, line);
    } while (line.find("NumRows") == string::npos);

    ss.clear();
    ss.str(line);

    ss >> strTmp >> strTmp >> nRows;

    while (line.find("CoreRow") == string::npos)
        getline(infile, line);
    for (int i = 0; i < nRows; i++)
    {
        Row tmp;
        int Coordinate, Height, Sitespacing, SubrowOrigin, NumSites;
        while (line.find("Coordinate") == string::npos)
            getline(infile, line);
        tmp.Coordinate = stoi(line.substr(line.find(":") + 1));

        while (line.find("Height") == string::npos)
            getline(infile, line);
        tmp.Height = stoi(line.substr(line.find(":") + 1));
        while (line.find("Sitespacing") == string::npos)
            getline(infile, line);
        // cout << line.substr(line.find(":") + 1) << endl;
        tmp.Sitespacing = stoi(line.substr(line.find(":") + 1));
        while (line.find("SubrowOrigin") == string::npos)
            getline(infile, line);

        // cout << "SubrowOrigin:" << line.substr(line.find(":") + 1, line.find("NumSites")) << endl;
        tmp.SubrowOrigin = stoi(line.substr(line.find(":") + 1, line.find("NumSites")));
        line.erase(0, line.find("NumSites") + 8);

        tmp.NumSites = stoi(line.substr(line.find(":") + 1));
        getline(infile, line);

        tmp.Width = 0;
        tmp.Right = tmp.SubrowOrigin + tmp.NumSites * tmp.Sitespacing;
        rows.push_back(tmp);

        // nTotalWidth = nTotalWidth + tmp.Sitespacing * tmp.NumSites;
        // nMinWidth = min(nMinWidth, tmp.Sitespacing * tmp.NumSites);
        // if (tmp.Coordinate > nMaxHeight)
        //     nMaxHeight = tmp.Coordinate;
        // if (tmp.Coordinate < nMinHeight)
        //     nMinHeight = tmp.Coordinate;
    }
    infile.close();
    // for (map<string, Terminal>::iterator i = terminals.begin(); i != terminals.end(); i++)
    // {
    //     cout << i->second.x1 << ": " << i->second.y1 << endl;
    // }

    // for (int i = 0; i < rows.size(); i++)
    // {
    //     cout << rows[i].Coordinate << " " << rows[i].SubrowOrigin << " " << rows[i].Right << endl;
    // }

    // ========== Fixed the Specific Nodes
    for (map<string, Terminal>::iterator it = terminals.begin(); it != terminals.end(); it++)
    {
        Terminal &tmp_T = it->second;

        for (int i = 0; i < rows.size(); ++i)
        {
            // cout << rows[i].Coordinate << " " << rows[i].SubrowOrigin << " " << rows[i].Right << " | " << tmp_T.x1 << " " << tmp_T.y1 << " " << tmp_T.w << " " << tmp_T.h << endl;

            if (rows[i].Coordinate >= (tmp_T.y1 + tmp_T.h))
                continue;
            if ((rows[i].Coordinate + rows[i].Height) <= tmp_T.y1)
                continue;
            // Fixed in the row
            if (rows[i].SubrowOrigin < tmp_T.x1 && rows[i].Right > (tmp_T.x1 + tmp_T.w))
            {
                int rr = rows[i].Right;
                rows[i].Right = tmp_T.x1;
                rows[i].NumSites = (rows[i].Right - rows[i].SubrowOrigin) / rows[i].Sitespacing;

                // Divide the Row to two rows this is the right one
                Row tmp;
                tmp.Coordinate = rows[i].Coordinate;
                tmp.Height = rows[i].Height;
                tmp.Sitespacing = rows[i].Sitespacing;
                tmp.SubrowOrigin = tmp_T.x1 + tmp_T.w;
                tmp.NumSites = (rr - tmp_T.x1 - tmp_T.w) / rows[i].Sitespacing;
                tmp.Right = rr;
                tmp.Width = 0;
                rows.push_back(tmp);
            }
            // Fixed at left edge
            else if (tmp_T.x1 <= rows[i].SubrowOrigin && (tmp_T.x1 + tmp_T.w) > rows[i].SubrowOrigin)
            {
                rows[i].SubrowOrigin = tmp_T.x1 + tmp_T.w;
                rows[i].NumSites = (rows[i].Right - rows[i].SubrowOrigin) / rows[i].Sitespacing;
            }
            // Fixed at right edge
            else if ((tmp_T.x1 + tmp_T.w) >= rows[i].Right && tmp_T.x1 < rows[i].Right)
            {
                rows[i].Right = tmp_T.x1;
                rows[i].NumSites = (rows[i].Right - rows[i].SubrowOrigin) / rows[i].Sitespacing;
            }
        }
    }
}
void Abacus()
{
    cout << "Abacus" << endl;
    // Lagalization for Ordered and non-Fixed cells

    for (int i = 0; i < cells.size() && difftime(time(NULL), t_1) < timelimit; i++)
    {

        // PrintCell(cells[i]);
        // cout << "After Calculation" << endl;
        Cell &c = cells[i];
        int BestRow, mid = 0;
        double cBest = numeric_limits<double>::max();
        // Find Mid Row
        for (int j = 0; j < rows.size(); j++)
        {
            if (c.y1 > rows[j].Coordinate)
            {
                mid = j;
                continue;
            }
            else
                break;
        }

        // cout << "Go Up From:" << mid << " " << rows[mid].Coordinate << endl;
        // Go upward
        for (int j = mid; j < rows.size(); j++)
        {

            // Set Limits
            if (min(abs(rows[j].Right - c.x1), abs(rows[j].SubrowOrigin - c.x1)) > cBest || abs(rows[j].Coordinate - c.y1) > cBest)
                break;
            if (rows[j].SubrowOrigin > c.x1 || c.x1 > rows[j].Right || (rows[j].Right - rows[j].SubrowOrigin - rows[j].Width) < c.w)
                continue;
            // Cannot Place in

            rows[j].nodes.push_back(i);
            c.y2 = rows[j].Coordinate;

            int store_x = c.x1;
            if (c.x1 < rows[j].SubrowOrigin)
                c.x1 = rows[j].SubrowOrigin;
            if ((c.x1 + c.w) > rows[j].Right)
                c.x1 = rows[j].Right - c.w;

            PlaceRow(rows[j]);
            // ======== Dynamic Programming
            double cost = pow(pow(c.x2 - c.x1, 2) + pow(c.y2 - c.y1, 2), 0.5);
            if (cost < cBest)
            {
                cBest = cost;
                BestRow = j;
            }
            rows[j].nodes.pop_back();
            c.x1 = store_x;
        }
        // cout << "Go Down" << endl;

        // Go downwward
        for (int j = mid - 1; j >= 0; j--)
        {
            // Set Limits
            if (min(abs(rows[j].Right - c.x1), abs(rows[j].SubrowOrigin - c.x1)) > cBest || abs(rows[j].Coordinate - c.y1) > cBest)
                break;
            if (rows[j].SubrowOrigin > c.x1 || c.x1 > rows[j].Right || (rows[j].Right - rows[j].SubrowOrigin - rows[j].Width) < c.w)
                continue;

            rows[j].nodes.push_back(i);
            c.y2 = rows[j].Coordinate;
            int store_x = c.x1;
            if (c.x1 < rows[j].SubrowOrigin)
                c.x1 = rows[j].SubrowOrigin;
            if ((c.x1 + c.w) > rows[j].Right)
                c.x1 = rows[j].Right - c.w;
            PlaceRow(rows[j]);
            // ======== Dynamic Programming
            double cost = pow(pow(c.x2 - c.x1, 2) + pow(c.y2 - c.y1, 2), 0.5);
            if (cost < cBest)
            {
                cBest = cost;
                BestRow = j;
            }
            rows[j].nodes.pop_back();
            c.x1 = store_x;
        }

        Row &chosed_r = rows[BestRow];
        chosed_r.nodes.push_back(i);
        c.y2 = chosed_r.Coordinate;
        // Move bigger than Left Edge
        if (c.x1 < chosed_r.SubrowOrigin)
            c.x1 = chosed_r.SubrowOrigin;

        // Move smaller than Right Edge
        if ((c.x1 + c.w) > chosed_r.Right)
            c.x1 = chosed_r.Right - c.w;

        PlaceRow(chosed_r);
        chosed_r.Width += c.w;
        // cout << "i: " << i << " ";
        // PrintCell(cells[i]);
    }
}
void PlaceRow(Row &r)
{
    // cout << "PlaceRow" << endl;
    for (int i = 0; i < r.nodes.size() && difftime(time(NULL), t_1) < timelimit; i++)
    {
        // cout << "i: " << i << endl;
        Cell &c = cells[r.nodes[i]];
        int c_last = r.clusters.size() - 1;
        // ======== Get Last Cell from Cluster
        // if (!r.clusters.empty())
        //     endptr = &r.clusters.back();

        if (i == 0 || (c_last >= 0 && (r.clusters[c_last].chosed_x + r.clusters[c_last].Width) < c.x1))
        {
            Cluster tmp_cluster;
            tmp_cluster.chosed_x = c.x1;
            tmp_cluster.Firstcell = i;

            // Addcell
            tmp_cluster.Lastcell = i;
            tmp_cluster.Weight = 1; // assume weight 1
            tmp_cluster.Quadratic = c.x1;
            tmp_cluster.Width = c.w;

            r.clusters.push_back(tmp_cluster);
        }
        else
        {
            int tmp = r.clusters.size() - 2; // predessesor

            // Addcell
            r.clusters[c_last].Lastcell = i;
            r.clusters[c_last].Weight += 1; // assume weight 1
            r.clusters[c_last].Quadratic += (c.x1 - r.clusters[c_last].Width);
            r.clusters[c_last].Width += c.w;

            Collapse(r.clusters[c_last], r.SubrowOrigin, r.Right, tmp, r.clusters);
        }
    }

    // ======== Trace All Clusters to Change legal x
    int i = 0;
    for (int it = 0; it < r.clusters.size(); it++)
    {
        int x = r.clusters[it].chosed_x;
        for (; i <= r.clusters[it].Lastcell; i++)
        {
            cells[r.nodes[i]].x2 = x;
            x = x + cells[r.nodes[i]].w;
        }
    }
    r.clusters.clear();
}
void Collapse(Cluster &cluster, int left, int right, int tmp, vector<Cluster> &clusters)
{

    if (difftime(time(NULL), t_1) > timelimit)
        return;
    // cout << "Collapse" << endl;
    cluster.chosed_x = cluster.Quadratic / cluster.Weight;

    if (cluster.chosed_x < left)
        cluster.chosed_x = left;
    if ((cluster.chosed_x + cluster.Width) > right)
        cluster.chosed_x = right - cluster.Width;
    if (tmp >= 0 && (clusters[tmp].chosed_x + clusters[tmp].Width) >= cluster.chosed_x) // Merge
    {
        // AddCluster
        clusters[tmp].Lastcell = cluster.Lastcell;
        clusters[tmp].Weight += cluster.Weight;
        clusters[tmp].Quadratic += cluster.Quadratic - cluster.Weight * clusters[tmp].Width;
        clusters[tmp].Width += cluster.Width;

        clusters.pop_back();
        tmp = clusters.size() - 2;
        Collapse(clusters.back(), left, right, tmp, clusters);
    }
}
// void Collapse Cluster &cluster, int left, int right, int tmp, vector<Cluster> &clusters)
// {
// }
void PrintCell(Cell c)
{
    cout << "str: " << c.str << " (" << c.x1 << ", " << c.y1 << ") (" << c.x2 << ", " << c.y2 << ") w: " << c.w << " h: " << c.h << endl; // Legal
}
