#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int main() {
    
    int a[100];
    int b[100];
    int index[50];
    int skip[50];
    int n;

    std::cin >> n;

    for (int i = 0; i< n; i++) {
        if (!skip[i]) {
            a[index[i]] = 0;
        }
        b[index[i]] = index[i];
    }

    return 0;
}
