#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int main() {
    int a[10][10][10][5];
    int b[4][5];

    int i;
    int k = 0;
    int j = 1;

    for (int m = 0; m < 5; m++) {
        i = 1;
        a[k][j][i][m] = a[k][j][i][m] - 100;
//        *
//          (5.0*b[i][m] - 4.0*b[i+1][m] +b[i+2][m]);
        i = 2;
        a[k][j][i][m] = a[k][j][i][m] - 200;
//
//        *
//          (-4.0*b[i-1][m] + 6.0*b[i][m] -
//            4.0*b[i+1][m] + b[i+2][m]);
    }

    return 0;
}
