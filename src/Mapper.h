#ifndef MAPPER_H
#define MAPPER_H

#include <pthread.h>
#include <map>
#include <string>
#include <vector>

class Mapper {
public:
    int id;
    pthread_barrier_t *reducer_barrier;

    std::map<std::string, int> &result;

    std::vector<std::string> &files;
    std::vector<pthread_mutex_t*> &file_mutexes;
    std::vector<bool> &parsed_file;


public:
    // Constructor
    Mapper(int id, pthread_barrier_t *reducer_barrier, std::map<std::string, int> &result,
            std::vector<std::string> &files, std::vector<pthread_mutex_t*> &file_mutexes,
            std::vector<bool> &parsed_file);

    // Destructor
    ~Mapper();


    void execute_map();
};


#endif /* MAPPER_H */
