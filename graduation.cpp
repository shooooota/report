#include <iostream>
#include <chrono>

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <valarray>
#include <fstream>
#include <string>
#include <sstream>
#include <numeric>
#include <cmath>

#include "/usr/local/include/opencv4/opencv2/core/core.hpp"
#include "/usr/local/include/opencv4/opencv2/flann.hpp"
#include <opencv2/core/types_c.h>
using namespace std;

struct Order_sigen_vector
{
    int order_player;
    float abs_cluster;

    Order_sigen_vector(int inputted_order, float inputted_abs)
    {
        order_player = inputted_order;
        abs_cluster = inputted_abs;
    }
    bool operator<(const Order_sigen_vector &another) const
    {
        return abs_cluster < another.abs_cluster; //年齢を比較
    };
    //2つのconstを付けないとコンパイルエラーになるので注意!!
};

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

// void print(const vector<int> &all_player)
// {
//     for_each(all_player.begin(), all_player.end(), [](int x) {
//         cout << x << " ";
//     });
//     cout << endl;
// }

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
    std::chrono::system_clock::time_point start, start_lp, end;
    start = std::chrono::system_clock::now();

    //////// 人数入力//////////
    const int num_player = 15; // プレイヤーの人数入力/
    int n_clusters = 40;        //クラスタ数
    /////////////////////////////
    const int players = (1 << num_player) -1;  //15人の協力ゲーム

    //ファイルの代入 出力
    ofstream data_file("30_20/p15_teian_custer40.csv");
    ifstream cparam("30_20/real_c.csv");
    ifstream param("30_20/real_a.csv");
    ifstream sparam("30_20/real_b.csv");

    // ofstream data_file("10_15/p14_teian_custer25.csv");
    // ifstream cparam("10_15/real_c_10_15.csv");
    // ifstream  param("10_15/real_a_10_15.csv");
    // ifstream sparam("10_15/real_b_10_15.csv");

    // ofstream data_file("10_5/p12_teian_custer15.csv");
    // ifstream cparam("10_5/real_c_10_5.csv");
    // ifstream param("10_5/real_a_10_5.csv");
    // ifstream sparam("10_5/real_b_10_5.csv");

    //変数と成約個数を求めている
    auto pos = sparam.tellg();

    string line, line2;
    getline(cparam, line);
    getline(sparam, line2);
    vector<int> limit = split(line, ',');
    vector<int> coeff = split(line2, ',');
    const int m = (int)limit.size();
    const int n = (int)coeff.size();
    coeff.clear();
    sparam.seekg(pos);

    vector<ifstream::pos_type> sigen_pos, a_pos;

    variable smallest_b, smallest_p;
    vector<int> A;
    vector<double> saitekiti(players + 1, 0.0);
    vector<int> cluster_count(n_clusters, 0);

    double B[m + 1][m + 1];
    int nonbasic[n], basic[m], nonbasic_row;
    double a, cbar, kari, smallest_cbar;
    double pbar[m + 1], p[m], p_kari[m], ans[n];
    double teikei[m + n];

    double saiteki_kai[n], zas;

    for (int i = 0; i < n; i++)
    {
        a_pos.push_back(param.tellg());
        getline(param, line);
    }

    //クラスタ分けに資源ベクトルを代入
    //確定形の先頭アドレスを読み込む
    cv::Mat samples = cv::Mat::zeros(players, n, CV_32FC1);
    for (int while_row = 0; while_row < players; while_row++)
    {
        // cout << while_row << endl;
        sigen_pos.push_back(sparam.tellg());
        getline(sparam, line);
        vector<int> b_vector = split(line, ',');

        float sum = accumulate(b_vector.begin(), b_vector.end(), 0.0);
        // cout << sum << endl;
        for (size_t i = 0; i < b_vector.size(); i++)
        {
            samples.at<float>(while_row, i) = (float)b_vector.at(i) / sum;
            // samples.at<double>(while_row, i) = b_vector.at(i);
            // cout << samples.at<float>(while_row, i) << " " ;
        }
        // cout << endl;
    }

    // kmeans法の適用
    cv::Mat clusters = cv::Mat::zeros(samples.rows, 1, CV_32F);
    cv::Mat centers;
    cv::kmeans(samples,
               n_clusters,
               clusters,
               cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 60, 0.001),
               20,
               cv::KMEANS_RANDOM_CENTERS,
               centers);

    end = chrono::system_clock::now();
    double elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    data_file << "計測時間(ms): " << elapsed << endl;

    // printf("clusters=%d\n", clusters.rows);
    vector<int> clustersat;
    for (int i = 0; i < clusters.rows; i++)
    {
        clustersat.push_back(clusters.at<int>(i));
        // printf("%d ", clusters.at<int>(i));
    }

    // for (int i = 0; i < n_clusters; i++)
    // {
    //     data_file << count(clustersat.begin(), clustersat.end(), i) << ",";
    // }
    // data_file << endl;
    printf("\n");
    // return 0;
    //     cluster_count[clusters.at<int>(i)] += 1;
    //   print(cluster_count);

    // printf("centers=\n");
    //   for (int i = 0; i < centers.rows; i++)
    //   {
    //     for (int j = 0; j < centers.cols; j++)
    //     {
    //       printf("%f ", centers.at<float>(i, j));
    //     }
    //     printf("\n");
    //   }
    //   return 0;

    // cout << info_oreder_player[0].oreder_player << endl;

    auto print = [](const float &n) { std::cout << " " << n; };

    /////////////////最適解の計算////////////////////////////

    /////////////////all cluster player////////////////////////////
    start_lp = std::chrono::system_clock::now();
    for (int cluster = 0; cluster < n_clusters; cluster++)
    {
        // data_file << cluster << endl;

        // std::vector<Order_sigen_vector> info_oreder_player;
        vector<float> abs_vector;
        vector<int> player_vector;

        for (int player = 0; player < players; player++)
        {
            if (clusters.at<int>(player) == cluster)
            {
                std::vector<float> nums;

                sparam.seekg(sigen_pos[player], ios_base::beg);
                getline(sparam, line2);
                coeff = split(line2, ',');

                //正規化している
                for (size_t i = 0; i < coeff.size(); i++)
                {
                    nums.push_back(coeff[i]);
                }
                float sum1 = accumulate(nums.begin(), nums.end(), 0);
                std::for_each(nums.begin(), nums.end(), [sum1](float &n) { n /= sum1; });

                float asd = 0.0;
                for (int i = 0; i < n; i++)
                {
                    // cout << nums[i] << " " << centers.at<float>(cluster, i) << endl;
                    asd += fabs(nums[i] - centers.at<float>(cluster, i));
                    // data_file << nums[i] << "," ;
                }
                // data_file << endl;

                // cout << asd <<endl;
                abs_vector.push_back(asd);
                player_vector.push_back(player);

                // info_oreder_player.push_back(Order_sigen_vector(player, asd));

                // std::for_each(nums.begin(), nums.end(), print);
                // std::cout << '\n';

                coeff.clear();
            }
        }
        // data_file << endl;

        std::vector<float>::iterator minIt = std::min_element(abs_vector.begin(), abs_vector.end());
        size_t minIndex = std::distance(abs_vector.begin(), minIt);

        // if (*player_vector == minIndex)
        // {
        //     cout << player_vector[minIndex] << endl;
        // }

        // std::sort(info_oreder_player.begin(), info_oreder_player.end()); //ソート実行

        // for (auto itr : info_oreder_player)
        // {
        //     cout << "距離:" << itr.abs_cluster << " player:" << itr.oreder_player << endl;
        // }

        // return 0;

        // if(cluster == 1) return 0;
        // cout << cluster << endl;
        // data_file << "クラスタ,  " << cluster << endl;

        // int kando_flag = 0;
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
            B[col][m] = -(double)(limit.at(col));
        }
        for (int col = 0; col < m + 1; col++)
        {
            B[m][col] = 0.0;
        }

        //基底の初期化
        for (int i = 0; i < n; i++)
        {
            nonbasic[i] = i;
        }
        for (int i = 0; i < m; i++)
        {
            basic[i] = n + i;
        }

        /////////////////改訂双対シンプレックス法////////////////////////////
        // if (clusters.at<int>(player) == cluster && kando_flag == 0)
        {
            // kando_flag++;
            // cout << "  " << player << endl;
            // data_file << "プレイヤー, " << player << "  ";

            //資源ベクトルの初期化
            sparam.seekg(sigen_pos[player_vector[minIndex]], ios_base::beg);
            getline(sparam, line2);
            coeff = split(line2, ',');

            for (int col = 0; col < n; ++col)
            {
                teikei[col] = (double)(coeff.at(col));
            }
            for (int col = n; col < m + n; ++col)
            {
                teikei[col] = 0.0;
            }
            coeff.clear();

            //計算
            int cou = 0;
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
                    for (int i = 0; i < n; i++)
                    {
                        saiteki_kai[i] = 0.0;
                    }
                    zas = 0.0;

                    for (int i = 0; i < m; i++)
                    {
                        zas += teikei[basic[i]] * B[i][m];
                        if (basic[i] < n)
                        {
                            saiteki_kai[basic[i]] = B[i][m];
                        }
                    }
                    // cout << cou << endl;

                    saitekiti[player_vector[minIndex] + 1] = -B[m][m];
                    // data_file << -B[m][m] << ", ,";
                    // data_file << "B[m][m]の値, " << -B[m][m] << " ";
                    // data_file << "基底解と資源ベクトルを掛けた値の値； " << zas << "  ";
                    // data_file << "変数yの解, ";

                    // for (int i = 0; i < n; i++)
                    // {
                    //     data_file << saiteki_kai[i] << ",";
                    // }
                    // data_file << *minIt << "," << cou;
                    // data_file << endl;

                    goto PAST;
                }
                cou++;

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
                            p_kari[j] = -(double)(A.at(j));
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
                for (int j = 0; j < m; j++)
                {
                    pbar[j] = 0.0;
                    for (int k = 0; k < m; k++)
                    {
                        pbar[j] += (B[j][k] * p[k]);
                    }
                    if (pbar[j] < 0.00001 && pbar[j] > -0.00001)
                        pbar[j] = 0.0;
                }
                pbar[m] = smallest_cbar;

                for (int i = 0; i < m + 1; i++)
                {
                    B[smallest_b.label][i] /= pbar[smallest_b.label];
                }

                for (int i = 0; i < m + 1; i++)
                {
                    if (i != smallest_b.label)
                    {
                        for (int j = 0; j < m + 1; j++)
                        {
                            B[i][j] -= B[smallest_b.label][j] * pbar[i];
                            if (B[i][j] < 0.00001 && B[i][j] > -0.00001)
                                B[i][j] = 0.0;
                        }
                    }
                }

                nonbasic[nonbasic_row] = basic[smallest_b.label];
                basic[smallest_b.label] = smallest_p.label;
            }
        PAST:;
        }
        ////////////////////感度分析/////////////////////////////////////
        vector<int> num_kanndo_count;
        auto abs_itr = abs_vector.begin();
        for (auto player = player_vector.begin(); player != player_vector.end(); ++player)
        {
            if (player_vector[minIndex] != *player)
            {
                // data_file << "プレイヤー:," << *player << "  ";

                //資源ベクトルの初期化
                sparam.seekg(sigen_pos[*player], ios_base::beg);
                getline(sparam, line2);
                coeff = split(line2, ',');

                for (int col = 0; col < n; ++col)
                {
                    teikei[col] = (double)(coeff.at(col));
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
                int kando_count = 0;

                double B_2[m + 1][m + 1];
                int nonbasic_2[n], basic_2[m];

                for (int row = 0; row < m + 1; ++row)
                {
                    for (int col = 0; col < m + 1; ++col)
                    {
                        B_2[row][col] = B[row][col];
                    }
                }

                //基底の初期化
                for (int i = 0; i < n; i++)
                {
                    nonbasic_2[i] = nonbasic[i];
                }
                for (int i = 0; i < m; i++)
                {
                    basic_2[i] = basic[i];
                }

                //計算
                while (true)
                {
                    double kando_z = 0.0, kando_min = 9999.9;

                    for (int non_j = 0; non_j < n; non_j++)
                    {
                        kando_c_j = 0.0;

                        //kando_c_jとkando_pbarを求めている
                        if (nonbasic_2[non_j] < n)
                        {
                            param.seekg(a_pos[nonbasic_2[non_j]], ios_base::beg);
                            getline(param, line);
                            A = split(line, ',');

                            for (int j = 0; j < m; j++)
                            {
                                kando_p[j] = -(double)(A.at(j));
                            }
                            A.clear();

                            for (int j = 0; j < m; j++)
                            {
                                kando_p_bar[j] = 0.0;
                                for (int k = 0; k < m; k++)
                                {
                                    kando_p_bar[j] += B_2[j][k] * kando_p[k];
                                }
                            }

                            for (int b_i = 0; b_i < m; b_i++)
                            {
                                kando_c_j += B_2[m][b_i] * kando_p[b_i];
                            }

                            kando_c_j += teikei[nonbasic_2[non_j]];
                        }
                        else
                        {
                            kando_c_j = B_2[m][nonbasic_2[non_j] - n];
                            for (int j = 0; j < m; j++)
                            {
                                kando_p_bar[j] = B_2[j][nonbasic_2[non_j] - n];
                            }
                        }

                        if (kando_min > kando_c_j)
                        {
                            kando_non_label = nonbasic_2[non_j];
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
                        // if(kando_count > 0) cout << cluster << " " << kando_count << endl;

                        for (int i = 0; i < n; i++)
                        {
                            saiteki_kai[i] = 0.0;
                        }
                        zas = 0.0;

                        for (int i = 0; i < m; i++)
                        {
                            zas += teikei[basic_2[i]] * B_2[i][m];
                            if (basic_2[i] < n)
                            {
                                saiteki_kai[basic_2[i]] = B_2[i][m];
                            }
                        }

                        saitekiti[*player + 1] = -B_2[m][m];
                        // data_file << -B_2[m][m] << ", ,";
                        // for (int i = 0; i < n; i++)
                        // {
                        //     data_file << saiteki_kai[i] << ",";
                        // }
                        // data_file << "感度分析 " << "B[m][m]の値, " << -B[m][m] << "  ";
                        // data_file << "基底解と資源ベクトルを掛けた値の値； " << zas << "  ";
                        // data_file << "距離, " ;

                        // data_file << " ,";
                        // data_file << kando_count << endl;
                        // data_file << *abs_itr << "," << kando_count << endl;

                        num_kanndo_count.push_back(kando_count);
                        // data_file << "変数yの解: " ;
                        // data_file << "ピポット操作, " ;

                        // data_file << "変数yの解, " ;

                        // if(kando_count != 0) cout << kando_count << endl;
                        // cout << "感度分析後のB[m][m]の値； " << B[m][m] << endl;
                        goto KANDO;
                    }
                    // else if(true){
                    //     goto KANDO;
                    // }
                    else
                    {
                        kando_count++;
                        double kando_min_warizan = 9999.9;
                        for (int label = 0; label < m; label++)
                        {
                            if (kando_min_warizan > (B_2[label][m] / kando_col[label]) && kando_col[label] > 0.00001)
                            {
                                kando_min_warizan = B_2[label][m] / kando_col[label];
                                kando_min_label = label;
                            }
                        }

                        for (int i = 0; i < m + 1; i++)
                        {
                            B_2[kando_min_label][i] /= kando_col[kando_min_label];
                        }

                        for (int i = 0; i < m + 1; i++)
                        {
                            if (i != kando_min_label)
                            {
                                for (int j = 0; j < m + 1; j++)
                                {
                                    B_2[i][j] -= B_2[kando_min_label][j] * kando_col[i];
                                    if (B_2[i][j] < 0.00001 && B_2[i][j] > -0.00001)
                                        B_2[i][j] = 0.0;
                                }
                            }
                        }
                        nonbasic_2[non_row] = basic_2[kando_min_label];
                        basic_2[kando_min_label] = kando_non_label;
                    }
                }
            KANDO:;
                ++abs_itr;
            }
        }

        // data_file << endl;

        // for (int i = 0; i < 11; i++)
        // {
        //     data_file << count(num_kanndo_count.begin(), num_kanndo_count.end(), i) << ",";
        // }
        // data_file << count_if(num_kanndo_count.begin(), num_kanndo_count.end(), [](int i) -> bool { return (i >= 11); }) << endl;

        // data_file << endl;
    }
    data_file << endl;
    // return 0;

    end = chrono::system_clock::now();
    elapsed = chrono::duration_cast<chrono::milliseconds>(end - start_lp).count();
    data_file << "計測時間(ms): " << elapsed << endl;
    elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    data_file << "計測時間(ms): " << elapsed << endl;
    cout << elapsed << endl;

    ///////////////シャープレイ値の計算//////////////////

    // vector<int> all_player;
    // for (int i = 0; i < num_player; i++)
    // {
    //     all_player.push_back(i);
    // }

    // for (int i = 0; i < 102; i++)
    // {
    //     cout << saitekiti[i] << endl;
    // }
    // return 0;

    // for (int i = 0; i < num_player; i++)
    // {
    //     double sharplay = 0;
    //     do
    //     {
    //         sharplay += func(all_player, i, saitekiti);
    //     } while (next_permutation(all_player.begin(), all_player.end()));
    //     // data_file << "プレイヤー: " << i << "　　　sharplay value : " << sharplay / kaijou(num_player) << endl;
    // }

    return 0;
}