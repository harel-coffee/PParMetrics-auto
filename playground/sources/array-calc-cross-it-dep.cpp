#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// arrays to conduct experiments on
static unsigned int a[100];
static unsigned int b[100];
static unsigned int c[100];

int main() {

    for (unsigned int i = 1; i < 100; i++) {
        c[i] = a[i] + b[i];
        a[i] = c[i-1];
    }

    return 0;
}
