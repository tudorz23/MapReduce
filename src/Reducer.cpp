#include "Reducer.h"

#include <iostream>
#include <fstream>

using namespace std;


Reducer::Reducer(int id, int start, int end, pthread_barrier_t *reducer_barrier,
                 std::vector<std::map<std::string, std::set<int>>> &mappers_result):
                mappers_result(mappers_result) {
    this->id = id;
    this->start = start;
    this->end = end;
    this->reducer_barrier = reducer_barrier;
    this->mappers_result = mappers_result;
}


Reducer::~Reducer() {
}


void Reducer::execute_reduce() {
    // Wait until all the mappers finish their work.
    pthread_barrier_wait(reducer_barrier);

    printf("Hello from reducer <%d>: ", id);
    printf("I take care of letters from %c to %c.\n", 'a' + start, 'a' + end);

    // if (id == 0) {
    //     ofstream out_file("partial_result.txt");
    //
    //     out_file << "Reducer 0 checks mappers results.\n";
    //     for (int i = 0; i < mappers_result.size(); i++) {
    //         out_file << "Results of mapper[" << i << "]:\n";
    //
    //         for (const auto &[key, value] : mappers_result[i]) {
    //             out_file << key << ": [";
    //
    //             for (auto &nr : value) {
    //                 out_file << nr << ", ";
    //             }
    //             out_file << "]\n";
    //         }
    //         out_file <<"\n";
    //     }
    //
    //     out_file.close();
    // }
}
