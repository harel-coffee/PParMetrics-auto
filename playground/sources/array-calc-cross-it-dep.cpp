#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

static const unsigned int size = 100;
static const unsigned int range = 10;

int main() {
    unsigned int a[size];
    unsigned int b[size];
    unsigned int c[size];

    std::srand(std::time(nullptr));

    // initialization loop
    for (unsigned int i = 0; i < size; i++) {
        a[i] = rand() % range;
        b[i] = rand() % range; 
    }

    for (unsigned int i = 1; i < 100; i++) {
        c[i] = a[i] + b[i];
        a[i] = c[i-1];
    }

    return 0;
}
