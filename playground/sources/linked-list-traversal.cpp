#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

// arrays to conduct experiments on
typedef struct list_node {
    list_node() : value(0), next(nullptr) {}
    int value;
    list_node* next;
} list_node_t;

static unsigned int size = 100;

int main() {

    list_node_t begin;
    list_node_t* list_it;

    list_it = &begin;
    for (unsigned int i = 1; i < size; i++) {
        list_it->next = new list_node_t;
        list_it->value = i;
        list_it = list_it->next;
    }
    
    int sum = 0;
    list_it = &begin;
    while (list_it != nullptr) {
        sum += list_it->value;
        list_it = list_it->next;
    }
    
    list_node_t* prev_it;
    list_it = &begin;
    while (list_it->next != nullptr) {
        prev_it = list_it;
        list_it = list_it->next;
        if (prev_it != &begin) delete prev_it;
    }
    delete list_it;

    cout << sum;

    return 0;
}
