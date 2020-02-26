#include <iostream>
#include <chrono>

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <valarray>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

int kaijou(int n)
{
    int sum = 1;
    for (int i = 1; i <= n; i++)
    {
        sum *= i;
    }
    return sum;
}

double func(const vector<int> &all_player, int player, const vector<double> &cooperation)
{
    int sum = 0;
    double sharplay;

    for (auto it = begin(all_player); it != end(all_player); ++it)
    {
        sum += (1 << *it);
        if (*it == player)
            break;
    }
    // cout << sum << " " << (sum & ~(1 << player)) << endl;
    // cout << cooperation[sum] - cooperation[(sum & ~(1 << player))] << endl;
    // cout << endl;

    return cooperation[sum] - cooperation[(sum & ~(1 << player))];
}

struct variable
{
    int label;
    double value;
};

struct eta
{
    int col;
    vector<double> values;
};

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

double ko = -100000;

int main(int argc, const char *argv[])
{
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();

    // プレイヤーの人数
    int o = 15;
    vector<double> saitekiti(1024, 0.0);

    // 代入している

    ofstream data_file("30_20/p15_dual_revised.csv");
    ifstream cparam("30_20/real_c.csv");
    ifstream param("30_20/real_a.csv");
    ifstream sparam("30_20/real_b.csv");

    // ofstream data_file("10_15/dual_revised_pivot_count.csv");
    // ifstream cparam("10_15/real_c_10_15.csv");
    // ifstream param("10_15/real_a_10_15.csv");
    // ifstream sparam("10_15/real_b_10_15.csv");

    // ofstream data_file("10_5/dual_revised_pivot_count.csv");
    // ifstream cparam("10_5/real_c_10_5.csv");
    // ifstream param("10_5/real_a_10_5.csv");
    // ifstream sparam("10_5/real_b_10_5.csv");

    // 提携Sの初期位置
    // 制約行列の初期位置
    auto pos = sparam.tellg();
    auto a_pos = param.tellg();
    auto c_pos = cparam.tellg();

    // n,m,を求めている
    vector<double> in;
    string line, line2;

    getline(cparam, line);
    vector<string> limit = split(line, ',');
    const int m = (int)limit.size();

    getline(sparam, line2);
    vector<string> coeff = split(line2, ',');
    const int n = (int)coeff.size();
    coeff.clear();

    // 提携Sの初期位置へ移動
    sparam.seekg(pos);
    cparam.seekg(c_pos);

    // タブローの定義B   基底b
    double B[m + 1][m + 1];

    double a, cbar, kari, smallest_cbar;
    double pbar[m + 1], p[m], p_kari[m], ans[n];

    int nonbasic[n], basic[m], nonbasic_row;
    int i, j, k, count, count_a = 0;

    //
    variable smallest_b, smallest_p;

    double teikei[m + n];

    vector<string> A;

    // 全提携計算
    for (int bit = 1; bit < (1 << o); ++bit)
    // while(getline(sparam, line2))
    {
        getline(sparam, line2);
        //cout << bit << endl;
        count = 0;
        // getline(sparam, line2);
        coeff = split(line2, ',');

        // 目的関数のcoefficientを求めている。
        for (int col = 0; col < n; ++col)
        {
            teikei[col] = stod(coeff.at(col));
        }
        for (int col = n; col < m + n; ++col)
        {
            teikei[col] = 0.0;
        }
        coeff.clear();

        // 基底非規定の初期化
        for (i = 0; i < n; i++)
        {
            nonbasic[i] = i;
        }
        for (i = 0; i < m; i++)
        {
            basic[i] = n + i;
        }

        // タブロー他の初期化
        for (int row = 0; row < m; ++row)
        {
            for (int col = 0; col < m; ++col)
            {
                if (col != row)
                {
                    B[row][col] = 0.0;
                }
                else
                {
                    B[col][col] = 1.0;
                }
            }
        }
        for (int col = 0; col < m; col++)
        {
            B[col][m] = -stod(limit.at(col));
        }
        for (int col = 0; col < m + 1; col++)
        {
            B[m][col] = 0.0;
        }

        while (true)
        {

            // step1 step2
            smallest_b.value = -0.00001;
            for (i = 0; i < m; i++)
            {
                if (B[i][m] < smallest_b.value)
                {
                    // if (count == 100) cout << B[i][m] << " ";
                    smallest_b = {i, B[i][m]};
                }
            }
            // cout << smallest_b.value << " ";
            // cout << B[m][m] << endl;
            // cout << endl;

            // cout << B[col][m] << endl;
            if (smallest_b.value == -0.00001
                //|| count == 70
            )
            {
                for (i = 0; i < n; i++)
                {
                    ans[i] = 0.0;
                }
                for (i = 0; i < m; i++)
                {
                    if (basic[i] < n)
                    {
                        ans[basic[i]] = B[i][m];
                    }
                }
                saitekiti[bit] = -B[m][m];
                data_file << count << endl;
                // data_file << -B[m][m] << ", ,";
                // for ( i = 0; i < n; i++)
                // {
                //     data_file << ans[i] << ",";
                // }
                // data_file << "\n";
                // cout << 1;
                // return 0;
                goto PAST;
            }

            count++;
            // step3 step4
            smallest_p.value = 99999.0;
            param.seekg(a_pos);
            count_a = 0;
            for (i = 0; i < n; i++)
            {
                a = 0.0;
                cbar = 0.0;
                getline(param, line);
                A = split(line, ',');
                for (j = 0; j < m; j++)
                {
                    p_kari[j] = -stod(A.at(j));
                    //if(count == 12) cout << p_kari[j] << " ";
                }
                A.clear();
                // return 0;
                // cout << endl;

                if (nonbasic[i] >= n)
                {
                    a = B[smallest_b.label][nonbasic[i] - n];
                    cbar = teikei[nonbasic[i]] + B[m][nonbasic[i] - n];
                    //  cout << a << " " << cbar << endl;
                }
                else
                {
                    for (j = 0; j < m; j++)
                    {
                        a += B[smallest_b.label][j] * p_kari[j];
                        cbar += B[m][j] * p_kari[j];
                    }
                    cbar += teikei[nonbasic[i]];
                    //  cout << a << " " << cbar << endl;
                }

                // if (count  == 11)
                // {
                //     cout << a << " ";
                //     // return 0;
                // }
                if (a < -0.00001)
                {
                    count_a++;
                    kari = cbar / -a;
                    // if (count == 41) cout << a << " ";
                    if (smallest_p.value > kari)
                    {
                        // cout << a << " " ;
                        smallest_p = {nonbasic[i], kari};
                        nonbasic_row = i;
                        smallest_cbar = cbar;
                        if (nonbasic[i] >= n)
                        {
                            for (j = 0; j < m; j++)
                            {
                                p[j] = 0;
                            }
                            p[nonbasic[i] - n] = 1;
                        }
                        else
                        {
                            for (j = 0; j < m; j++)
                            {
                                p[j] = p_kari[j];
                            }
                        }
                        // if( smallest_p.label >= n) cout << smallest_p.label << endl;
                    }
                }
            }
            if (count_a == 0)
            {
                // for ( i = 0; i < m; i++)
                // {
                //     // cout << a;
                //     ans[i] = 0.0;
                //     if (basic[i] < n)
                //     {
                //         ans[basic[i]] = B[i][m];
                //     }
                // }
                // data_file << -B[m][m] << ", ,";
                // for ( i = 0; i < n; i++)
                // {
                //     data_file << ans[i] << ",";
                // }
                // data_file << "\n";
                // return 0;
                // cout << count << endl;
                return 0;
            }
            // cout << smallest_p.value << " " << smallest_cbar << " ";
            // cout << endl;
            //if(count == 12) cout << smallest_p.value << " ";

            for (i = 0; i < m; i++)
            {
                //if(count == 12) cout << p[i] << " ";
            }

            // cout << endl;

            // step5
            //for ( i = 0; i < n; i++)
            {
                //if (nonbasic[i] < n)
                {
                    for (j = 0; j < m; j++)
                    {
                        pbar[j] = 0.0;
                        for (k = 0; k < m; k++)
                        {
                            pbar[j] += (B[j][k] * p[k]);
                        }
                        if (pbar[j] < 0.00001 && pbar[j] > -0.00001)
                            pbar[j] = 0.0;
                        if (count == 100)
                            cout << pbar[j] << " ";
                        // if(count == 4 && j == 0) cout << p[k] << " ";
                        // if(count == 4 && j == 0) cout << B[j][k] << " ";
                        // if(count == 4 && j == 0) cout << B[j][k] * p[k] << endl;
                        // if(count == 4 && j == 0) cout << pbar[j] << " ";
                        // if(count == 4 && j == 0) cout << pbar[j] << endl;
                        // //cout << endl;
                    }
                    // if(count == 100) cout << endl;
                }
                // cout << endl;
                // else
                // {
                //     for ( j = 0; j < m; j++)
                //     {
                //         pbar[j] = B[j][nonbasic[i] - n];
                //     }
                // }
            }

            pbar[m] = smallest_cbar;
            // for ( j = 0; j < m + 1; j++)
            // {
            //     // cout << pbar[j] << " ";
            // }
            // cout << endl;

            // ピポット操作
            //if(count == 12) cout << smallest_b.label << " ";
            for (i = 0; i < m + 1; i++)
            {
                //if(count == 12) cout << B[smallest_b.label][i] << " ";
                //pbar[i] /= pbar[smallest_b.label];
                B[smallest_b.label][i] /= pbar[smallest_b.label];
            }
            // cout << endl;

            for (i = 0; i < m + 1; i++)
            {
                if (i != smallest_b.label)
                {
                    for (j = 0; j < m + 1; j++)
                    {
                        B[i][j] -= B[smallest_b.label][j] * pbar[i];
                        if (B[i][j] < 0.00001 && B[i][j] > -0.00001)
                            B[i][j] = 0.0;
                    }
                    // cout << B[i][m] << " ";
                }
            }
            // cout << endl;
            // if(bit == 28)
            {
                // if(ko < B[m][m] ) cout << 123 << " ";
                // cout << count << " " << B[m][m] << endl;
            }
            //  if(bit == 29) return 0;
            // cout << endl;
            //  if (count == 56) return 0;

            // cout << smallest_b.label << endl;
            // cout << smallest_p.label << endl;

            nonbasic[nonbasic_row] = basic[smallest_b.label];
            basic[smallest_b.label] = smallest_p.label;
            // cout << basic[smallest_b.label] << endl;
            // if(ko < B[m][m] ) cout << nonbasic[nonbasic_row] << " ";
            // if(ko < B[m][m] ) cout << basic[smallest_b.label] << " ";
            ko = B[m][m];
            // cout << endl;
        }
    PAST:;
    }

    vector<int> all_player;
    for (int i = 0; i < o; i++)
    {
        all_player.push_back(i);
    }

    // for (int i = 0; i < 102; i++)
    // {
    //     cout << saitekiti[i] << endl;
    // }
    // return 0;

    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    data_file << "ms. " << elapsed << endl;

    for (int i = 0; i < o; i++)
    {
        double sharplay = 0;
        do
        {
            sharplay += func(all_player, i, saitekiti);
        } while (next_permutation(all_player.begin(), all_player.end()));
        // data_file << "プレイヤー: " << i << "　　　sharplay value : " << sharplay / kaijou(o) << endl;
    }
}
