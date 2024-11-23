#ifndef REDUCER_H
#define REDUCER_H

#include <pthread.h>
#include <vector>
#include <map>
#include <string>
#include <set>


class Reducer {
private:
    int id;

    char start_char;
    char end_char;

    pthread_barrier_t *reducer_barrier;

    std::vector<std::map<std::string, std::set<int>>> &mappers_result;

public:
    // Constructor.
    Reducer(int id, char start_char, char end_char, pthread_barrier_t *reducer_barrier,
            std::vector<std::map<std::string, std::set<int>>> &mappers_result);

    // Destructor.
    ~Reducer();

    // Main method of a Reducer.
    void execute_reduce();
};

#endif /* REDUCER_H */
