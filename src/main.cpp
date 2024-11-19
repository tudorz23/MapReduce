#include <iostream>

#include <pthread.h>
#include <vector>
#include <map>

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


void change_map(map<string, int> &testMap) {
    map<string, int> &likeClass = testMap;

    cout << "From func: " << likeClass["tudor"] << "\n";
    likeClass["tudor"] = 14;
    cout << "From func: " << likeClass["tudor"] << "\n";
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

    // Barrier that doesn't allow reducers to start before all the mappers finish.
    pthread_barrier_t reducer_barrier;
    pthread_barrier_init(&reducer_barrier, NULL, mappers_cnt + reducers_cnt);

    // Create the mappers result array.
    vector<map<string, int>> mappers_result;
    for (int i = 0; i < mappers_cnt; i++) {
        map<string, int> result;
        mappers_result.push_back(result);
    }

    // Create mapper objects.
    vector<Mapper*> mappers;
    for (int i = 0; i < mappers_cnt; i++) {
        Mapper *mapper = new Mapper(i, &reducer_barrier, mappers_result[i]);
        mappers.push_back(mapper);
    }

    // Create reducer objects.
    vector<Reducer*> reducers;
    for (int i = 0; i < reducers_cnt; i++) {
        Reducer *reducer = new Reducer(i, &reducer_barrier, mappers_result);
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

    // Free resources.
    delete[] threads;

    for (long unsigned int i = 0; i < mappers.size(); i++) {
        delete mappers[i];
    }

    for (long unsigned int i = 0; i < reducers.size(); i++) {
        delete reducers[i];
    }

    pthread_barrier_destroy(&reducer_barrier);


    for (int i = 0; i < mappers_cnt; i++) {
        cout << "Results of mapper [" << i << "]: ";
        cout << "mapper_" << i << "[tudor] = " << mappers_result[i]["tudor"] << "\n";
    }

    return 0;
}
