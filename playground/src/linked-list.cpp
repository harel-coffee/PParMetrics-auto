#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

typedef struct list_node {
    list_node() : value(0), next(nullptr) {}
    int value;
    list_node* next;
} list_node_t;

static unsigned int size = 100;

int main() {
   
    list_node_t nodes[100];
    list_node_t* list_it;

    unsigned int i = 0;
    for (; i < size-1; i++) {
        nodes[i].value = i;
        nodes[i].next = &nodes[i+1];
    }
    nodes[i].value = i;
    nodes[i].next = nullptr;

    int sum = 0;
    list_it = &nodes[0];
    while (list_it != nullptr) {
        sum += list_it->value;
        list_it = list_it->next;
    }
    
    cout << sum;

    return 0;
}
