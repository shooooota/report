#include <iostream>
#include <vector>
#include <algorithm>

// using namespace cv;

void print(const std::vector<int> &all_player)
{
    std::for_each(all_player.begin(), all_player.end(), [](int x) {
        std::cout << x << " ";
    });
    std::cout << std::endl;
}

double func(const std::vector<int> &all_player, int player, const std::vector<double> &cooperation)
{
    int sum = 0;
    double sharplay;

    for (auto it = std::begin(all_player); it != std::end(all_player); ++it)
    {
        sum += (1 << *it);
        if (*it == player)
            break;
    }
    std::cout << sum << " " << (sum & ~(1 << player)) << std::endl;
    std::cout << cooperation[sum] - cooperation[(sum & ~(1 << player))] << std::endl;
    std::cout << std::endl;

    return cooperation[sum] - cooperation[(sum & ~(1 << player))];
}

int kaijou(int n)
{
    int sum = 1;
    for (int i = 1; i <= n; i++)
    {
        sum *= i;
    }
    return sum;
}

int main()
{
    
    // プレイヤーの人数入力//////////
    const int num_player = 3;
    /////////////////////////////

    // 提携の最適値入力 /////////////
    std::vector<double> cooperation = {0, 5, 40, 55, 55, 65, 100, 120};
    ///////////////////////////////





    std::vector<int> all_player;
    for (int i = 0; i < num_player; i++)
    {
        all_player.push_back(i);
    }


    for (int i = 0; i < num_player; i++)
    {
        double sharplay = 0;
        do
        {
            print(all_player);
            sharplay += func(all_player, i, cooperation);
        } while (std::next_permutation(all_player.begin(), all_player.end()));
        std::cout << sharplay / kaijou(num_player) << std::endl;
    }
}
