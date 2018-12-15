#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

static const unsigned int size = 100;
static const unsigned int range = 10;

int main() {
    unsigned int a[size];

    for (unsigned int i = 0; i < size; i++) {
        a[i+1] = a[i] + 10;
    }

    return 0;
}
