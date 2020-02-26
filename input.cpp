#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
using namespace std;

vector<string> split(string& input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }
    return result;
}

int main()
{
    ifstream ifs("data2.csv");

    string line;
    while (getline(ifs, line)) {
        
        vector<string> strvec = split(line, ',');
        vector<double> in;
        for (int i=0; i<strvec.size();i++){
            printf("%f ", stod(strvec.at(i)));
        }
    }
} 
 
