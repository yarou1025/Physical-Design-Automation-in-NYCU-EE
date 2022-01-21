#include <iostream>
#include <fstream>
#include "File_IO.cpp"

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 2)
        cout << " No enough input file" << endl;
    else
    {
        FM myfm = ReadFile(string(argv[1]));
        // PrintAll(myfm);

        // ofstream out;
        // out.open("output.txt");
        // for (int j = 1; j <= myfm.c_size; j++)
        // {
        //     out << myfm.Result[j] << endl;
        // }
        // // cout<<endl;
        // out.close();

        // cout << "Min_Cuts:" << myfm.cutsize << endl;
    }
}