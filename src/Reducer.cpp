#include "Reducer.h"

#include <iostream>

using namespace std;


Reducer::Reducer(int id, pthread_barrier_t *reducer_barrier,
                std::vector<std::map<std::string, int>> &mappers_result):
                mappers_result(mappers_result) {
    this->id = id;
    this->reducer_barrier = reducer_barrier;
    this->mappers_result = mappers_result;
}

Reducer::~Reducer() {
}

void Reducer::execute_reduce() {
    // Wait until all the mappers finish their work.
    pthread_barrier_wait(reducer_barrier);

    printf("Hello from reducer <%d>.\n", id);

    if (id == 0) {
        printf("Reducer 0 checks mappers results.\n");
        for (int i = 0; i < mappers_result.size(); i++) {
            cout << "Results of mapper [" << i << "]: ";
            cout << "mapper_" << i << "[tudor] = " << mappers_result[i]["tudor"] << "\n";
        }
    }
}
