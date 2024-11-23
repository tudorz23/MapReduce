#ifndef MAPPER_H
#define MAPPER_H

#include <pthread.h>
#include <map>
#include <string>
#include <vector>
#include <set>


class Mapper {
private:
    int id;
    pthread_barrier_t *reducer_barrier;

    std::map<std::string, std::set<int>> &result;

    std::vector<std::string> &files;
    std::vector<pthread_mutex_t*> &file_mutexes;
    std::vector<bool> &parsed_file;

public:
    // Constructor.
    Mapper(int id, pthread_barrier_t *reducer_barrier,
            std::map<std::string, std::set<int>> &result,
            std::vector<std::string> &files,
            std::vector<pthread_mutex_t*> &file_mutexes,
            std::vector<bool> &parsed_file);

    // Destructor.
    ~Mapper();

    // Main method of a Mapper.
    void execute_map();

private:
    void parse_file(const int index);
};

#endif /* MAPPER_H */
