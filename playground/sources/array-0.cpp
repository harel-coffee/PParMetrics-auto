#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

static const unsigned int size = 100;
static const unsigned int range = 10;

int main() {
    unsigned int c[size];

    std::srand(std::time(nullptr));

    // initialization loop
    for (unsigned int i = 0; i < size; i++) {
        c[i] = rand() % range; 
    }

    for (unsigned int i = 2; i < size; i++) {
        c[i] = c[i-1] + c[i-2];
    }

    return 0;
}
