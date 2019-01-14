#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

void f(int a[], int b[], int n) {
    for (unsigned int i = 1; i < n; i++) {
        a[i] = b[i] + 5;
    }
}

int main() {
    int a[100];
    int b[100];

    f(a,b,100);

    return 0;
}
