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

int main {
    ifstream ifs("parameter.csv");
    string line, line2;
    vector<float> in;

    while (getline(ifs, line))
    {

        vector<string> strvec = split(line, ',');
        for (int i = 0; i < strvec.size(); i++)
        {
            in.push_back(stod(strvec.at(i)));
        }
    }




}