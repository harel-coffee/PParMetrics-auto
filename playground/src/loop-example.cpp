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
    unsigned int i;
    for ( i = 1, list_it = &begin; (i < size) && (list_it != nullptr); list_it = list_it->next, i++) {
        list_it->next = new list_node_t;
        list_it->value = i;
    }
    
    return 0;
}
