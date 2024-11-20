#include "Mapper.h"

#include <iostream>

using namespace std;


Mapper::Mapper(int id, pthread_barrier_t *reducer_barrier,
                std::map<std::string, int> &result, std::vector<std::string> &files,
                std::vector<pthread_mutex_t*> &file_mutexes,
                std::vector<bool> &parsed_file):
                result(result), files(files), file_mutexes(file_mutexes), parsed_file(parsed_file)
{
    this->id = id;
    this->reducer_barrier = reducer_barrier;
}


Mapper::~Mapper() {
}


void Mapper::execute_map() {
    // Dynamically choose a file to parse, starting from own id index.
    int parsed_cnt = 0;
    for (int i = id; i < files.size(); i++) {
        bool should_parse = false;

        pthread_mutex_lock(file_mutexes[i]);

        if (!parsed_file[i]) {
            should_parse = true;
            parsed_file[i] = true;
        }

        pthread_mutex_unlock(file_mutexes[i]);

        if (should_parse) {
            for (int j = 0; j < 10000000; j++) {
                continue;
            }
            parsed_cnt++;
            printf("Mapper %d parses file %d.\n", id, i + 1);
        }
    }

    printf("Mapper %d ---- parsed %d files.\n", id, parsed_cnt);

    // Signal that the work of this mapper is finished.
    pthread_barrier_wait(reducer_barrier);
}
