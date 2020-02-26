#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <valarray>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

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

double ko = -100000;

int main(int argc, const char *argv[])
{
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();

    int o = 10;

    ofstream data_file("30_20/kando_pivot_count.csv");
    ifstream cparam("30_20/real_c.csv");
    ifstream param("30_20/real_a.csv");
    ifstream sparam("30_20/real_b.csv");

    // ofstream data_file("10_15/kando_pivot_count.csv");
    // ifstream cparam("10_15/real_c_10_15.csv");
    // ifstream param("10_15/real_a_10_15.csv");
    // ifstream sparam("10_15/real_b_10_15.csv");

    // ofstream data_file("10_5/kando_pivot_count.csv");
    // ifstream cparam("10_5/real_c_10_5.csv");
    // ifstream param("10_5/real_a_10_5.csv");
    // ifstream sparam("10_5/real_b_10_5.csv");

    auto pos = sparam.tellg();
    auto a_pos = param.tellg();
    auto c_pos = cparam.tellg();

    vector<double> in;
    string line, line2;

    getline(cparam, line);
    vector<int> limit = split(line, ',');
    const int m = (int)limit.size();

    getline(sparam, line2);
    vector<int> coeff = split(line2, ',');
    const int n = (int)coeff.size();
    coeff.clear();

    sparam.seekg(pos);
    cparam.seekg(c_pos);

    double B[m + 1][m + 1];
    double a, cbar, kari, smallest_cbar;
    double pbar[m + 1], p[m], p_kari[m], ans[n];
    double teikei[m + n];

    int nonbasic[n], basic[m], nonbasic_row;
    int i, j, k, count, count_a = 0;

    variable smallest_b, smallest_p;
    vector<int> A;
    for (i = 0; i < n; i++)
    {
        nonbasic[i] = i;
    }
    for (i = 0; i < m; i++)
    {
        basic[i] = n + i;
    }

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

    getline(sparam, line2);
    count = 0;
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

    while (true)
    {

        smallest_b.value = -0.00001;
        for (i = 0; i < m; i++)
        {
            if (B[i][m] < smallest_b.value)
            {
                // if (count == 100)
                smallest_b = {i, B[i][m]};
            }
        }

        if (smallest_b.value == -0.00001)
        {
            // for (i = 0; i < m; i++)
            // {

            //     ans[i] = 0.0;
            //     if (basic[i] < n)
            //     {
            //         ans[basic[i]] = B[i][m];
            //     }
            // }
            double zas = 0.0;
            for (i = 0; i < m; i++)
            {
                // cout << B[m][i] << " ";
                zas += teikei[basic[i]] * B[i][m];
            }
            // cout << count << endl;
            data_file << count << endl;

            goto PAST;
            break;
        }
        count++;

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
                p_kari[j] = -(double)(A.at(j));
            }
            A.clear();

            if (nonbasic[i] >= n)
            {
                a = B[smallest_b.label][nonbasic[i] - n];
                cbar = teikei[nonbasic[i]] + B[m][nonbasic[i] - n];
            }
            else
            {
                for (j = 0; j < m; j++)
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
                }
            }
        }
        if (count_a == 0)
        {
        }

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
                if (B[i][j] < 0.00001 && B[i][j] > -0.00001)
                    B[i][j] = 0.0;
            }
        }

        nonbasic[nonbasic_row] = basic[smallest_b.label];
        basic[smallest_b.label] = smallest_p.label;

        ko = B[m][m];
    }

PAST:;

    // while (getline(sparam, line2))
    for (int bit = 2; bit < (1 << o); ++bit)
    {
        getline(sparam, line2);
        count = 0;
        // if(count == 5) return 0;
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

        double kando_p[m], kando_p_bar[m], kando_col[m + 1], kando_c_j;
        int kando_min_label, kando_non_label, non_row;

        for (int non_j = 0; non_j < m + 1; non_j++)
        {
            B[m][non_j] = 0.0;
            for (int b_i = 0; b_i < m; b_i++)
            {
                B[m][non_j] -= teikei[basic[b_i]] * B[b_i][non_j];
            }
            // cout << B[m][non_j]  << " ";
        }
        // cout << B[m][m] << endl;

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

        while (true)
        {
            double kando_z = 0.0, kando_min = 9999.9;
            for (int non_j = 0; non_j < n; non_j++)
            {
                kando_c_j = 0.0;
                // cout << nonbasic[non_j] << endl;

                if (nonbasic_2[non_j] < n)
                {

                    // cout << 1;
                    int ghkl = 0;
                    param.seekg(a_pos);
                    while (nonbasic_2[non_j] != ghkl)
                    {
                        ghkl++;
                        getline(param, line);
                    }
                    getline(param, line);
                    A = split(line, ',');
                    for (j = 0; j < m; j++)
                    {
                        kando_p[j] = -(double)(A.at(j));
                        // cout << kando_p[j] << " ";
                    }
                    A.clear();

                    for (j = 0; j < m; j++)
                    {
                        kando_p_bar[j] = 0.0;
                        for (k = 0; k < m; k++)
                        {
                            kando_p_bar[j] += B_2[j][k] * kando_p[k];
                        }
                    }

                    // for (int b_i = 0; b_i < m; b_i++)
                    // {
                    //     kando_c_j -= teikei[basic[b_i]] * kando_p_bar[b_i];
                    // }
                    for (int b_i = 0; b_i < m; b_i++)
                    {
                        kando_c_j += B_2[m][b_i] * kando_p[b_i];
                    }

                    kando_c_j += teikei[nonbasic_2[non_j]];
                    // cout << kando_c_j;
                }
                else
                {
                    kando_c_j = B_2[m][nonbasic_2[non_j] - n];
                    for (j = 0; j < m; j++)
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
                        // cout << kando_col[j] << " ";
                    }
                }
            }

            if (kando_min > -0.00001)
            {
                // for (int b_i = 0; b_i < m; b_i++)
                // {
                //     //if (basic_2[b_i] < n)
                //     {
                //         // cout << B_2[b_i][m] << " " << basic_2[b_i] << endl;
                //         kando_z += B_2[b_i][m] * teikei[basic_2[b_i]];
                //     }
                // }
                // cout << B_2[m][m] << " " << count << endl;
                data_file << count << endl;
                goto KANDO;
                break;
            }
            else
            {
                count++;
                // if(count == 4) return 0;
                double kando_min_warizan = 9999.9;
                for (int label = 0; label < m; label++)
                {
                    if (kando_min_warizan > (B_2[label][m] / kando_col[label]) && kando_col[label] > 0.00001)
                    {
                        kando_min_warizan = B_2[label][m] / kando_col[label];
                        kando_min_label = label;
                    }
                }
                // return 0;
                // cout << kando_min_label << " " << endl;
                // return 0;

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
                // cout << B[m][m] << endl;
                nonbasic_2[non_row] = basic_2[kando_min_label];
                basic_2[kando_min_label] = kando_non_label;
            }
        }
    KANDO:;
    }
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    data_file << elapsed << endl;
}
