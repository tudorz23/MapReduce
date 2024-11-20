#include "Mapper.h"

#include <iostream>

using namespace std;


Mapper::Mapper(int id, pthread_barrier_t *reducer_barrier,
               std::map<std::string, int> &result): result(result) {
    this->id = id;
    this->reducer_barrier = reducer_barrier;
}


Mapper::~Mapper() {
}


void Mapper::execute_map() {
    printf("Hello from mapper <%d>.\n", id);

    result["tudor"] = this->id * 3;

    // Signal that the work of this mapper is finished.
    pthread_barrier_wait(reducer_barrier);
}
