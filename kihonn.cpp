
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib> 
#include <valarray>
using namespace std;


int main()
{
    static size_t m = 4;

    struct variable {
        size_t label;
        int value;
    };

    struct ea {
        size_t col;
        vector<double> values;
    };

    ea b[4], S[16];
    

    srand((unsigned int)time(NULL));

    /* bを求めている*/
    for (size_t l = 0; l < m; l++){
        for (int i = 0; i < 4; i++){
            b[l].values.push_back(rand()%100);
        }
    }
    int n = 4;
    for (int bit = 1; bit < (1<<n); ++bit)
    {
        /* 
        bit で表される集合の処理を書く 
         きちんとできていることを確認してみる 
         */
        // bit の表す集合を求める
        vector<int> s;
        for (int i = 0; i < n; ++i) {
            if (bit & (1<<i)) { // i が bit に入るかどうか
                s.push_back(i);
            }
        }

        // bit の表す集合の出力
        for (int i = 0; i < (int)s.size(); ++i) {
            vector<double> sum;
            for(auto itr = b[s[i]].values.begin(); itr != b[s[i]].values.end(); ++itr) {
                sum.push_back(*itr);
            }
            for(size_t i = 0; i < m; i++){
                S[bit].values[i] += sum[i];
            }
        }
        for(size_t i = 0; i < m; i++){
                printf("%10.3f ", S[bit].values[i]);
        }
    }
    return 0;

    
}
