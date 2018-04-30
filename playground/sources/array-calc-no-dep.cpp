#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;


int main() {

    // arrays to conduct experiments on
    unsigned int a[100];
    unsigned int b[100];
    unsigned int c[100];

    for (unsigned int i = 0; i < 100; i++) {
        c[i] = a[i] + b[i];
    }

    return 0;
}
