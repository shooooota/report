#include <unordered_map>       // ヘッダファイルインクルード
#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
using namespace std;         //  名前空間指定




int main()
{
    unordered_map<int, int> mp;       // ローカル変数として、mp を生成
    mp[1] = 78;
    cout << mp[1] << endl;
    mp[1] = 93;
    cout << mp[1] << endl;
    mp[0] = 48;

    vector<int> v{3, 4, 5, 9, 2, 6, 5, 3, 5};
    reverse(v.begin(), v.end());
    for(auto x : v ) cout << x << " ";
    cout << endl;
    decltype(v)::iterator i = std::min(v.begin(), v.end());
    cout << *i << endl;

    array<int, 10> av{3, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    decltype(av)::iterator ai = std::min(av.begin(), av.end());
    cout << *ai << endl;
    sort(av.begin(), av.end());       // 順序反転
    for(auto x : av ) cout << x << " ";
    cout << "\n";
}