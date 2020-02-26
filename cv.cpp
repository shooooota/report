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

int main()
{
  cv::Mat samples = cv::Mat::zeros(1023, 30, CV_32FC1);
  // samples.at<float>(0, 0) = 100.0;
  // samples.at<float>(1, 0) = 100.0;
  // samples.at<float>(2, 0) = 80.0;
  // samples.at<float>(3, 0) = 70.0;
  // samples.at<float>(4, 0) = 50.0;
  // std::cout << "samples=\n"
  //           << samples << std::endl;

  ifstream ifs("real_b^40.csv");
  string line, line2;

  // auto file_pos[1023];
  // file_pos.push_back(ifs.tellg());

  vector<ifstream::pos_type> file_pos;
  // for(int i = 0; i < 40; i++){
  // }
  // ifs.seekg(file_pos[0], ios_base::beg);
  // getline(ifs, line);
  // cout << line << endl;


  // while(getline(ifs, line)){
  // }
  
  
  // getline(ifs, line);

  // file_pos.push_back(ifs.tellg());
  // getline(ifs, line);
  // file_pos.push_back(ifs.tellg());
  // getline(ifs, line);
  // // auto pos = ifs.tellg();

  // getline(ifs, line);
  // getline(ifs, line);
  

  // getline(ifs, line);
  // cout << sizeof(line) << endl;

  // cout << sizeof(line) << endl;



  
  // int while_row = 0;
  // while (getline(ifs, line))
    for(int while_row = 0; while_row < 40; while_row++)
    {
        file_pos.push_back(ifs.tellg());
        getline(ifs, line);
        // cout << line << endl;
        // ifs.seekg(file_pos[while_row], ios_base::beg);
        // getline(ifs, line);
        //  cout << line << endl;

        // file_pos.push_back(ifs.tellg());

        vector<int> strvec = split(line, ',');
        float sum = accumulate(strvec.begin(), strvec.end(), 0.0);
        // cout << sum;
        for (size_t i = 0; i < strvec.size(); i++)
        {
            samples.at<float>(while_row, i) = (float)(strvec.at(i)/sum);
            // cout << samples.at<float>(while_row, i) ;
        }
        // while_row++;
        // cout << endl;
    }

  

  int n_clusters = 10;
  cv::Mat clusters = cv::Mat::zeros(samples.rows, 1, CV_32F);

  cv::Mat centers;
  cv::kmeans(samples,
             n_clusters,
             clusters,
             cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 0.0001),
             1,
             cv::KMEANS_RANDOM_CENTERS,
             centers);

  //std::cout << clusters << std::endl;
  printf("clusters=\n");
  for (int i = 0; i < clusters.rows; i++)
  {
    printf("%d ", clusters.at<int>(i));
  }
  printf("\n");

  
  // std::cout << centers[9] << std::endl;
  printf("centers=\n");
  for (int i = 0; i < centers.rows; i++)
  {
    for (int j = 0; j < centers.cols; j++)
    {
      printf("%f ", centers.at<float>(i, j));
    }
    printf("\n");
  }

  // file_pos--;
  //   ifs.seekg(file_pos[1], ios_base::beg);
  //   getline(ifs, line);
  //   cout << line << endl;


  return 0;
}