#ifndef REDUCER_H
#define REDUCER_H

#include <pthread.h>
#include <vector>
#include <map>
#include <string>


class Reducer {
public:
    int id;
    pthread_barrier_t *reducer_barrier;

    std::vector<std::map<std::string, int>> &mappers_result;



public:
    // Constructor
    Reducer(int id, pthread_barrier_t *reducer_barrier,
            std::vector<std::map<std::string, int>> &mappers_result);

    // Destructor
    ~Reducer();

    void execute_reduce();
};



#endif /* REDUCER_H */
