#include <iostream>

#include <vector>
#include <pthread.h>

#include "Mapper.h"
#include "Reducer.h"

using namespace std;


void *execute_map(void *arg) {
    Mapper *mapper = (Mapper*) arg;

    mapper->execute_map();

    pthread_exit(NULL);
}


void *execute_reduce(void *arg) {
    Reducer *reducer = (Reducer*) arg;

    reducer->execute_reduce();

    pthread_exit(NULL);
}


int main(int argc, char **argv) {
    // Return code used for every function return code check.
    int r;

    // Get the number of mappers and the number of reducers.
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <MAPPERS_CNT> <REDUCERS_CNT> <INPUT_FILE>\n";
        exit(-1);
    }

    int mappers_cnt;
    r = sscanf(argv[1], "%d", &mappers_cnt);
    if (r != 1) {
        fprintf(stderr, "Invalid number of mappers.\n");
        exit(-1);
    }

    int reducers_cnt;
    r = sscanf(argv[2], "%d", &reducers_cnt);
    if (r != 1) {
        fprintf(stderr, "Invalid number of reducers.\n");
        exit(-1);
    }

    // Allocate memory for the array of threads.
    pthread_t *threads;

    try {
        threads = new pthread_t[mappers_cnt + reducers_cnt];
    } catch (bad_alloc &exception) {
        fprintf(stderr, "Allocation failed.\n");
        exit(-1);
    }

    // Create mapper objects.
    vector<Mapper*> mappers;
    for (int i = 0; i < mappers_cnt; i++) {
        Mapper *mapper = new Mapper(i);
        mappers.push_back(mapper);
    }

    // Create reducer objects.
    vector<Reducer*> reducers;
    for (int i = 0; i < reducers_cnt; i++) {
        Reducer *reducer = new Reducer(i);
        reducers.push_back(reducer);
    }

    // Launch the worker threads.
    for (int i = 0; i < mappers_cnt + reducers_cnt; i++) {
        if (i < mappers_cnt) {
            // Create a mapper thread.
            r = pthread_create(&threads[i], NULL, execute_map, mappers[i]);
        } else {
            // Create a reducer thread.
            r = pthread_create(&threads[i], NULL, execute_reduce, reducers[i - mappers_cnt]);
        }

        if (r) {
            fprintf(stderr, "Error creating thread %d.\n", i);
            exit(-1);
        }
    }

    // Join the worker threads.
    for (int i = 0; i < mappers_cnt + reducers_cnt; i++) {
        r = pthread_join(threads[i], NULL);

        if (r) {
            fprintf(stderr, "Error waiting for thread %d.\n", i);
            exit(-1);
        }
    }

    delete[] threads;

    for (long unsigned int i = 0; i < mappers.size(); i++) {
        delete mappers[i];
    }

    for (long unsigned int i = 0; i < reducers.size(); i++) {
        delete reducers[i];
    }

    return 0;
}
