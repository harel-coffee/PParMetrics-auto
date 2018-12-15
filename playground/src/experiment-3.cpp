#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

static const unsigned int size = 100;
static const unsigned int range = 10;

void f(unsigned int* a, unsigned int size) {

    for (unsigned int i = 0; i < size; i++) {
        a[i+3] = a[i] + 10;
    }

    return;
}

int main() {
    unsigned int a[size];

    for (unsigned int i = 0; i < 5; i++) {
        for (unsigned int i = 0; i < size; i++) {
            a[i+7] = a[i] + 10;
        }
    }

    for (unsigned int i = 0; i < 5; i++) {
        f(a, size);
    }

    return 0;
}
