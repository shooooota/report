#include <iostream>  // for cout
#include <stdio.h>   // for printf()

#define N_ROW 4      // 行数
#define N_COL 6      // 列数
#define N_VAR 2      // 変数の数

using namespace std;

/*
 * 計算クラス
 */
class Calc
{
    // 各種変数
    double min, p, d;
    int i, j, k, x, y, flag, m, l, n;

    public:
        void calcLinearProgramming();   // 線形計画法
};

/*
 * 線形計画法
 */
void Calc::calcLinearProgramming()
{
    // 係数行列
    static double a[N_ROW][N_COL+1] = {
        { 1.0,  2.0,  1.0,  0.0,  0.0, 14.0, 0.0},
        { 1.0,  1.0,  0.0,  1.0,  0.0,  8.0, 0.0},
        { 3.0,  1.0,  0.0,  0.0,  1.0, 18.0, 0.0},
        {-2.0, -3.0,  0.0,  0.0,  0.0,  0.0, 0.0}
    };

    while (1) {
        // 列選択
        min = 9999;
        for (k = 0; k < N_COL - 1; k++) {
            for (l = N_VAR;l < N_COL-1; l++){
                m = m + a[N_ROW-1][l]*a[k][l];
            }
            if (a[N_ROW - 1][k] - m < min) {
                min = a[N_ROW - 1][k] + m;
                y = k;
            }
        }
        if (min >= 0) break;
        for (l=0; l < N_ROW - 1; l++){
            for (k = N_VAR; k < N_COL-1; k++){
                n = n + a[l][k]*a[k - N_VAR][y];
            }
            a[l][N_COL] = n;
        }
        a[N_ROW - 1][N_COL] = a[N_ROW - 1][y];
        // 行選択
        min = 9999;
        for (k = 0; k < N_ROW - 1; k++) {
            p = a[k][N_COL - 1] / a[k][N_COL];
            if ( p < min) {
                min = p;
                x = k;
            }
        }

        // ピボット係数
        p = a[x][N_COL];

        // ピボット係数を p で除算
        for (k = 0; k < N_COL; k++)
            a[x][k] = a[x][k] / p;

        // ピボット列の掃き出し
        for (k = 0; k < N_ROW; k++) {
            if (k != x) {
                d = a[k][N_COL];
                for (j = 0; j < N_COL; j++)
                    a[k][j] = a[k][j] - d * a[x][j];
            }
        }
    }

    // 結果出力
    for (k = 0; k < N_VAR; k++) {
        flag = -1;
        for (j = 0; j < N_ROW; j++) {
            // ==== 2016-11-14 UPDATE ===>
            // if (a[j][k] == 1) flag = j;
            if (a[j][k] == 1) {
                flag = j;
            } else if (flag != -1 && a[j][k] != 0) {
                flag = -1;
                break;
            }
            // <=== 2016-11-14 UPDATE ====
        }
        if (flag != -1)
            printf("x%d = %8.4f\n", k, a[flag][N_COL - 1]);
        else
            printf("x%d = %8.4f\n", k, 0.0);
    }
    printf("z  = %8.4f\n", a[N_ROW - 1][N_COL - 1]);
}

/*
 * メイン処理
 */
int main()
{
    try
    {
        // 計算クラスインスタンス化
        Calc objCalc;

        // 線形計画法
        objCalc.calcLinearProgramming();
    }
    catch (...) {
        cout << "例外発生！" << endl;
        return -1;
    }

    // 正常終了
    return 0;
}