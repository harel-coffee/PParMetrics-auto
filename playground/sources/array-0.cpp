#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

static const unsigned int size = 100000;
static const unsigned int range = 10;

int main() {
    unsigned int c[size];

    std::srand(std::time(nullptr));

    // initialization loop
    for (unsigned int i = 0; i < size; i++) {
        c[i] = rand() % range; 
    }

    int sum = 0;
    unsigned int i = 0;
    while(i < size) {
        sum += c[i++];
    }

    return sum;
}
