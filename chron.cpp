#include <iostream>
#include <chrono>

int main()
{
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();

    // for (size_t i = 0; i < 2345; i++)
    // {
    //     std::cout << i << std::endl;
    // }

    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << elapsed << std::endl;

    ////////////////////microseconds, nanoseconds, seconds
}