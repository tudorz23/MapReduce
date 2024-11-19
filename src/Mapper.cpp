#include "Mapper.h"

#include <pthread.h>
#include <iostream>


Mapper::Mapper(int id) {
    this->id = id;
}


Mapper::~Mapper() {
}


void Mapper::execute_map() {
    printf("Hello from mapper <%d>.\n", id);
}
