#include <fstream>

int main() {
  const int size_x = 4;
  const int size_y = 5;
  int u[size_x][size_y];
  for (int i = 0; i <size_x ; i++) {
    for (int j = 0; j < size_y; j++) {
      u[i][j] = i*10+j;
    }
  }


  std::ofstream data_file("A.csv");
  for (int i = 0; i <size_x ; i++) {
    for (int j = 0; j < size_y; j++) {
      data_file << u[i][j] << ",";
    }
    data_file<< std::endl;
  }
  data_file.close();
}