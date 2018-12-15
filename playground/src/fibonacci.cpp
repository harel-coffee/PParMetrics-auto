#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

static unsigned int size = 100;

unsigned long int compute_fibonacci_num(int num) {
    unsigned long int num_pp = 0; // pre-previous
    unsigned long int num_p = 1; // previous
    unsigned long int fibb_num;
        
    fibb_num = num_p;
    for (unsigned int i = 1; i < num; i++) { 
        fibb_num = num_p + num_pp;
        num_pp = num_p;
        num_p = fibb_num;
    }
    
    return fibb_num;
}

int main() {
    int fibonacci_numbers[size];
    
    for (int i = 1; i < size; i++) {
        fibonacci_numbers[i-1] = compute_fibonacci_num(i);
    }
 
    cout << "Fibonacci numbers are:\n";
    for (int i = 0; i < size; i++) {
        cout << (i+1) << ": " << fibonacci_numbers[i] << endl;
    }
   
    return 0;
}
