#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

int main() {
    unsigned int a[100];

    for (unsigned int i = 0; i < 99; i++) {
        a[i] = i;
        a[i+1] = i+1;
    }

    return 0;
}
