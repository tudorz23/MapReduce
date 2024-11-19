#ifndef MAPPER_H
#define MAPPER_H

#include <pthread.h>
#include <map>
#include <string>

class Mapper {
public:
    int id;
    pthread_barrier_t *reducer_barrier;

    std::map<std::string, int> &result;


public:
    // Constructor
    Mapper(int id, pthread_barrier_t *reducer_barrier, std::map<std::string, int> &result);

    // Destructor
    ~Mapper();


    void execute_map();
};


#endif /* MAPPER_H */
