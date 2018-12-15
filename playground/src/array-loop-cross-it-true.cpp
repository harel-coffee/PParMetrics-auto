#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

int main() {
    unsigned int a[100];

    for (unsigned int i = 1; i < 100; i++) {
        a[i] = a[i-1];
    }

    return 0;
}
