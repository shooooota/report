#include <iostream>
#include "/usr/local/include/opencv4/opencv2/core/core.hpp"
#include "/usr/local/include/opencv4/opencv2/flann.hpp"
#include <opencv2/core/types_c.h>

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <valarray>
#include <fstream>
#include <string>
#include <sstream>
#include <numeric>
#include <cmath>
using namespace std;



vector<int> split(string &input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<int> result;
    while (getline(stream, field, delimiter))
    {
        result.push_back(stoi(field));
    }
    return result;
}


struct variable
{
    int label;
    double value;
};


void print(const vector<int> &all_player)
{
    for_each(all_player.begin(), all_player.end(), [](int x) {
        cout << x << " ";
    });
    cout << endl;
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

// int kaijou(int n)
// {
//     int sum = 1;
//     for (int i = 1; i <= n; i++)
//     {
//         sum *= i;
//     }
//     return sum;
// }


int main()
{
    //40人の協力ゲーム
    const int players = 1023;   
    //クラスタ数
    int n_clusters = 3;
        
    // プレイヤーの人数入力//////////
    const int num_player = 10;
    /////////////////////////////


    //ファイルの代入 出力
    ofstream data_file("kando.csv");

    ifstream cparam("real_c.csv");
    ifstream param("real_a.csv");
    ifstream sparam("real_b.csv");



    //変数と成約個数を求めている
    auto pos = sparam.tellg();

    getline(cparam, line);
    getline(sparam, line2);
    vector<int> limit = split(line, ',');
    vector<int> coeff = split(line2, ',');
    const int m = (int)limit.size();
    const int n = (int)coeff.size();
    coeff.clear();
    sparam.seekg(pos);







    string line, line2;
    vector<ifstream::pos_type> sigen_pos, a_pos;

    variable smallest_b, smallest_p;
    vector<int> A;
    vector<double> saitekiti(players, 0.0);

    double B[m + 1][m + 1];
    double a, cbar, kari, smallest_cbar;
    double pbar[m + 1], p[m], p_kari[m], ans[n];
    double teikei[m + n];

    int nonbasic[n], basic[m], nonbasic_row;

    for (int i = 0; i < n; i++)
    {
        a_pos.push_back(param.tellg());
        getline(param, line);
    }



    //クラスタ分けに資源ベクトルを代入
    //確定形の先頭アドレスを読み込む
    cv::Mat samples = cv::Mat::zeros(players, 30, CV_32FC1);
    for(int while_row = 0; while_row < players; while_row++)
    {
        sigen_pos.push_back(sparam.tellg());
        getline(sparam, line);
        vector<int> b_vector = split(line, ',');

        double sum = accumulate(b_vector.begin(), b_vector.end(), 0);
        for (size_t i = 0; i < b_vector.size(); i++)
        {
            samples.at<double>(while_row, i) = (b_vector.at(i)/sum);
        }

    }

  
    // kmeans法の適用
    cv::Mat clusters = cv::Mat::zeros(samples.rows, 1, CV_32SC1);
    cv::Mat centers;
    cv::kmeans(samples,
             n_clusters,
             clusters,
             cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 0.01),
             1,
             cv::KMEANS_RANDOM_CENTERS,
             centers);


    /////////////////最適解の計算////////////////////////////
    /////////////////all cluster player////////////////////////////
    for(int cluster = 0; cluster < n_clusters; cluster++)
    {
        int kando_flag = 0;
        //逆行列初期化 B[][]
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

        //基底の初期化
        for (int i = 0; i < n; i++){
            nonbasic[i] = i;
        }
        for (int i = 0; i < m; i++)
        {
            basic[i] = n + i;
        }

        for(int player = 0; player < players; player++)
        {
            /////////////////改訂双対シンプレックス法////////////////////////////
            if(clusters.at<int>(player) == cluster && kando_flag == 0){
                kando_flag++;

                //資源ベクトルの初期化
                sparam.seekg(sigen_pos[player], ios_base::beg);
                getline(sparam, line2);
                coeff = split(line2, ',');
                for (int col = 0; col < n; ++col)
                {
                    teikei[col] = stod(coeff.at(col));
                }
                for (int col = n; col < m + n; ++col)
                {
                    teikei[col] = 0.0;
                }
                coeff.clear();


                //計算
                while (true)
                {
                    //////////////////step1/////////////////
                    smallest_b.value = -0.00001;
                    for (int i = 0; i < m; i++)
                    {
                        if (B[i][m] < smallest_b.value)
                        {
                            smallest_b = {i, B[i][m]};
                        }
                    }

                    if (smallest_b.value == -0.00001)
                    {
                        double zas=0.0;
                        for (int i = 0; i < m; i++)
                        {
                            zas += teikei[basic[i]] * B[i][m];
                        }
                        saitekiti[player] = B[m][m];
                        cout << "B[m][m]の値； " << B[m][m] << endl;
                        cout << "基底解と資源ベクトルを掛けた値の値； " << zas << endl;
                        
                        goto PAST;
                    }



                    //////////////////step2 3/////////////////
                    smallest_p.value = 99999.0;
                    int count_a = 0;
                    for (int i = 0; i < n; i++)
                    {
                        a = 0.0;
                        cbar = 0.0;

                        if (nonbasic[i] >= n)
                        {
                            a = B[smallest_b.label][nonbasic[i] - n];
                            cbar = teikei[nonbasic[i]] + B[m][nonbasic[i] - n];
                        }
                        else
                        {
                            param.seekg(a_pos[nonbasic[i]], ios_base::beg);
                            getline(param, line);
                            A = split(line, ',');
                            for (int j = 0; j < m; j++)
                            {
                                p_kari[j] = -stod(A.at(j));
                            }
                            A.clear();

                            for (int j = 0; j < m; j++)
                            {
                                a += B[smallest_b.label][j] * p_kari[j];
                                cbar += B[m][j] * p_kari[j];
                            }
                            cbar += teikei[nonbasic[i]];
                        }

                        if (a < -0.00001)
                        {
                            count_a++;
                            kari = cbar / -a;
                            if (smallest_p.value > kari)
                            {
                                smallest_p = {nonbasic[i], kari};
                                nonbasic_row = i;
                                smallest_cbar = cbar;
                                if (nonbasic[i] >= n)
                                {
                                    for (int j = 0; j < m; j++)
                                    {
                                        p[j] = 0;
                                    }
                                    p[nonbasic[i] - n] = 1;
                                }
                                else
                                {
                                    for (int j = 0; j < m; j++)
                                    {
                                        p[j] = p_kari[j];
                                    }
                                }
                            }
                        }
                    }
                    if (count_a == 0)
                    {
                        cout << "解なし発見" << endl;
                    }


                    //////////////////step4/////////////////
                    for (j = 0; j < m; j++)
                    {
                        pbar[j] = 0.0;
                        for (k = 0; k < m; k++)
                        {
                            pbar[j] += (B[j][k] * p[k]);
                        }
                        if (pbar[j] < 0.00001 && pbar[j] > -0.00001)
                            pbar[j] = 0.0;
                    }
                    pbar[m] = smallest_cbar;

                    for (i = 0; i < m + 1; i++)
                    {
                        B[smallest_b.label][i] /= pbar[smallest_b.label];
                    }

                    for (i = 0; i < m + 1; i++)
                    {
                        if (i != smallest_b.label)
                        {
                            for (j = 0; j < m + 1; j++)
                            {
                                B[i][j] -= B[smallest_b.label][j] * pbar[i];
                            }
                            if (B[i][j] < 0.00001 && B[i][j] > -0.00001) B[i][j] = 0.0;
                        }
                    }

                    nonbasic[nonbasic_row] = basic[smallest_b.label];
                    basic[smallest_b.label] = smallest_p.label;

                }
                PAST:;

            ////////////////////感度分析/////////////////////////////////////
            }else if(clusters.at<int>(player) == cluster)
            {

                //資源ベクトルの初期化
                sparam.seekg(sigen_pos[player], ios_base::beg);
                coeff = split(line2, ',');
                for (int col = 0; col < n; ++col)
                {
                    teikei[col] = stod(coeff.at(col));
                }
                for (int col = n; col < m + n; ++col)
                {
                    teikei[col] = 0.0;
                }
                coeff.clear();

                //b[m]ぎょうの最適基底解の変化
                for (int non_j = 0; non_j < m + 1; non_j++)
                {
                    B[m][non_j] = 0.0;
                    for (int b_i = 0; b_i < m; b_i++)
                    { 
                        B[m][non_j] -= teikei[basic[b_i]] * B[b_i][non_j];
                    }
                }

                double kando_p[m], kando_p_bar[m], kando_col[m + 1], kando_c_j;
                int kando_min_label, kando_non_label, non_row;

                //計算
                while (true)
                {
                    double kando_z = 0.0, kando_min=9999.9;

                    for (int non_j = 0; non_j < n; non_j++)
                    {
                        kando_c_j=0.0;

                        //kando_c_jとkando_pbarを求めている
                        if(nonbasic[non_j] < n)
                        {
                            param.seekg(a_pos[nonbasic[non_j]], ios_base::beg);
                            getline(param, line);
                            A = split(line, ',');
                            for (int j = 0; j < m; j++)
                            {
                                kando_p[j] = -stod(A.at(j));
                            }
                            A.clear();

                            for (int j = 0; j < m; j++)
                            {
                                kando_p_bar[j] = 0.0;
                                for (int k = 0; k < m; k++)
                                {
                                    kando_p_bar[j] += B[j][k] * kando_p[k];
                                }
                            }

                            for (int b_i = 0; b_i < m; b_i++)
                            {
                                kando_c_j += B[m][b_i] * kando_p[b_i];
                            }

                            kando_c_j += teikei[nonbasic[non_j]];
                        }
                        else
                        {
                            kando_c_j = B[m][nonbasic[non_j] - n];
                            for (j = 0; j < m; j++){
                                kando_p_bar[j] = B[j][nonbasic[non_j] - n];
                            }
                        }

                        if (kando_min > kando_c_j )
                        {
                            kando_non_label = nonbasic[non_j];
                            non_row = non_j;
                            kando_min = kando_c_j;
                            kando_col[m] = kando_c_j;
                            for (int j = 0; j < m; j++)
                            {
                                kando_col[j] = kando_p_bar[j];
                            }
                        }
                    }

                    //解の発見とシンプレックス法の適用
                    if (kando_min > -0.00001)
                    {
                        saitekiti[player] = B[m][m];
                        cout << "感度分析後のB[m][m]の値； " << B[m][m] << endl; 
                        goto KANDO;
                    }
                    else
                    {
                        double kando_min_warizan = 9999.9;
                        for(int label = 0; label < m; label++){
                            if(kando_min_warizan > (B[label][m]/kando_col[label]) && kando_col[label] > 0.00001){
                                kando_min_warizan = B[label][m]/kando_col[label];
                                kando_min_label = label;
                            }
                        }

                        for (int i = 0; i < m + 1; i++){
                            B[kando_min_label][i] /= kando_col[kando_min_label];
                        }

                        for (int i = 0; i < m + 1; i++){
                            if (i != kando_min_label){
                                for (int j = 0; j < m + 1; j++){
                                    B[i][j] -= B[kando_min_label][j] * kando_col[i];
                                    if (B[i][j] < 0.00001 && B[i][j] > -0.00001) B[i][j] = 0.0;
                                }
                            }
                        }
                        nonbasic[non_row] = basic[kando_min_label];
                        basic[kando_min_label] = kando_non_label;

                    }
                }
                KANDO:;
            }

        }

    }

    ///////////////シャープレイ値の計算//////////////////


    vector<int> all_player;
    for (int i = 0; i < num_player; i++)
    {
        all_player.push_back(i);
    }

    for (int i = 0; i < num_player; i++)
    {
        double sharplay = 0;
        do
        {
            sharplay += func(all_player, i, saitekiti);
        } while (next_permutation(all_player.begin(), all_player.end()));
        cout << "プレイヤー:  " << i << sharplay / kaijou(num_player) << endl;
    }

    return 0;
}