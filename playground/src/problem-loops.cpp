#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

class Array {
    
    public:    
        Array() 
        { 
            array = nullptr; 
            size = -1; 
        }

        Array(int s) 
        { 
            array = new int[s]; 
            size = s;
        }

        ~Array()
        {
            delete array;
        }

        int* array;
        int size;
};

int get_size() {
    return 50;
}

int main() {
    
    int size;
    std::cin >> size;

    Array a(size);
    
    unsigned int i = 0;
    for (int i = 0; i < a.size; i++) {
        a.array[i] = i;
    }

    for (int i = 0; i < 100; i++) {
        a.array[i] = i;
    }

    for (int i = 0; i < 100; i++) {
        if (a.array[i] != 0) {
            a.array[i] = i;
        } else {
            break;
        }
    }

    for (int i = 0; i < get_size()-25; i++) {
        a.array[i] = i;
    }

    return 0;
}
