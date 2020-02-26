#include <iostream>
#include <thread> // <thread>をインクルード
#include <vector>

int main()
{
    std::vector<int> v{ 0, 1, 2, 3, 4 };

    std::vector<std::thread> threads;

    // 各要素をスレッド別に処理
    for(std::size_t i = 0; i < 5; ++i) {
        threads.emplace_back([i, &v](){
            v[i] *= 2;
        });
    }

    // 全スレッドの同期待ち
    for(auto& t : threads) {
        t.join();
    }

    for(const auto& vi : v) {
        std::cout << vi << std::endl;
    }

    return 0;
}