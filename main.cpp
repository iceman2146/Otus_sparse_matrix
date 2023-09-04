#include "matrix.h"

using namespace std;
int main() {

  const int N = 10;

  Matrix<int, 0> matrix;

  for (int i = 0; i < N; i++) {
    matrix[i][i] = matrix[N - 1 - i][i] = i;
  }

  int n = 0;

  for (int i = 0; i < 8; i++)
    for (int j = i + 2; j < 10; j += 2) {
      matrix[i][j] = matrix[j][i] = 0;
      n++;
    }

  for (int i = 0; i < 5; i++) {
    matrix[i][i] = matrix[10 - i - 1][10 - i - 1] = 0;
    n++;
  }

  for (int i = 1; i <= 8; i++) {
    for (int j = 1; j <= 8; j++) {
      auto v = matrix[i][j];
      cout << matrix[i][j] << " ";
    }
    cout << endl;
  }

  cout << "occupied: " << matrix.size() << endl;

  for (auto item : matrix)
    cout << item.first << " = " << item.second << endl;

  return 0;
}