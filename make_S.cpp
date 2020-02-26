#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
using namespace std;

vector<string> split(string &input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter))
    {
        result.push_back(field);
    }
    return result;
}

struct player
{
    int col;
    int values[30];
};

int main()
{
    int o = 30;
    int m = 30;
    ofstream data_file("real_b^40.csv");
    ifstream ifs("real_b.csv");

    string line;
    vector<int> in;

    while (getline(ifs, line))
    {
        vector<string> strvec = split(line, ',');
        for (int i = 0; i < strvec.size(); i++)
        {
            in.push_back(stoi(strvec.at(i)));
        }
    }

    player x[o];
    for (size_t l = 0; l < o; l++)
    {
        for (size_t i = 0; i < m; i++)
        {
            x[l].values[i] = in[i + l * m]; 
        }
    }

    for (unsigned int bit = 1; bit < (1 << 31); ++bit)
    {

        cout << bit << endl;
        vector<int> s;
        for (int i = 0; i < o; ++i)
        {
            if (bit & (1 << i))
            { // i が bit に入るかどうか
                s.push_back(i);
            }
        }

        int S[m];
        for (int i = 0; i < m; i++)
        {
            S[i] = 0;
        }
        // bit の表す集合の出力
        //double SUM = 0.0;
        for (int i = 0; i < (int)s.size(); ++i)
        {
            for (int a = 0; a < m; a++)
            {
                S[a] += x[s[i]].values[a];
            }
        }

        for(int i = 0; i < m ; i++){
            data_file << S[i] << ",";
        }
        data_file << endl;
    }

    return 0;
}


// #include <iostream>
// #include <vector>
// using namespace std;

// int main() {
//     int n = 40;

//     // {0, 1, ..., n-1} の部分集合の全探索
//     for (int bit = 0; bit < (int)(1<<n); ++bit)
//     {
//         /* bit で表される集合の処理を書く */


//         /* きちんとできていることを確認してみる */
//         // bit の表す集合を求める
//         vector<int> S;
//         for (int i = 0; i < n; ++i) {
//             if (bit & (int)(1<<i)) { // i が bit に入るかどうか
//                 S.push_back(i);
//             }
//         }

//         // bit の表す集合の出力
//         cout << bit << ": {";
//         for (int i = 0; i < (int)S.size(); ++i) {
//             cout << S[i] << " ";
//         }
//         cout << "}" << endl;
//     }
// }