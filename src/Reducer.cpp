#include "Reducer.h"

#include <iostream>

Reducer::Reducer(int id) {
    this->id = id;
}

Reducer::~Reducer() {
}

void Reducer::execute_reduce() {
    printf("Hello from reducer <%d>.\n", id);
}
