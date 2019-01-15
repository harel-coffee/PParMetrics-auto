#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

#define LX1 100

int main() {
    double x1[LX1][LX1][LX1];
    double x2[LX1][LX1][LX1];
    double y[LX1][LX1][LX1];

    int ix, iz, ip;

    for (iz = 0; iz < LX1; iz++) {
        for (ix = 0; ix < LX1; ix++) {
            y[iz][0][ix] = x1[iz][0][ix];

            y[iz][1][ix] = 0.0;
/*
            for (ip = 0; ip < LX1; ip++) {
                y[iz][1][ix] = y[iz][1][ix] + 10*x1[iz][ip][ix];
            }*/

            y[iz][2][ix] = x1[iz][LX1-1][ix];

            /*
            y[iz][3][ix] = 0.0;
            for (ip = 0; ip < LX1; ip++) {
                y[iz][3][ix] = y[iz][3][ix] + 5*x2[iz][ip][ix];
            }

            y[iz][LX1-1][ix] = x2[iz][LX1-1][ix];*/
        }
    }

    return 0;
}
