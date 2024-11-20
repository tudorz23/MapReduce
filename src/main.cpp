#include <iostream>

#include <pthread.h>
#include <vector>
#include <map>
#include <fstream>

#include "Mapper.h"
#include "Reducer.h"

using namespace std;


// Thread function that "launches" a mapper.
void *execute_map(void *arg) {
    Mapper *mapper = (Mapper*) arg;

    mapper->execute_map();

    pthread_exit(NULL);
}


// Thread function that "launches" a reducer.
void *execute_reduce(void *arg) {
    Reducer *reducer = (Reducer*) arg;

    reducer->execute_reduce();

    pthread_exit(NULL);
}


// Open the file passed as command-line argument and read the input files.
void get_input_files(const string& user_file, vector<string> &files) {
    // Open the file.
    ifstream fin(user_file);

    int entries;
    fin >> entries;

    string line;

    for (int i = 0; i < entries; i++) {
        fin >> line;
        files.push_back(line);
    }

    fin.close();
}


int main(int argc, char **argv) {
    // Get the number of mappers and the number of reducers.
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <MAPPERS_CNT> <REDUCERS_CNT> <INPUT_FILE>\n";
        exit(-1);
    }

    // Return code used for every function return code check.
    int r;

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

    // Get the input files.
    vector<string> files;
    get_input_files(argv[3], files);


    // Create one mutex for each file to dynamically split them to mappers.
    vector<pthread_mutex_t*> file_mutexes;
    for (int i = 0; i < files.size(); i++) {
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, NULL);
        file_mutexes.push_back(&mutex);
    }

    vector<bool> parsed_file(files.size(), false);


    // Barrier that doesn't allow reducers to start before all the mappers finish.
    pthread_barrier_t reducer_barrier;
    pthread_barrier_init(&reducer_barrier, NULL, mappers_cnt + reducers_cnt);


    // Create the mappers result array, where each mapper will place its result.
    vector<map<string, int>> mappers_result;
    for (int i = 0; i < mappers_cnt; i++) {
        map<string, int> result;
        mappers_result.push_back(result);
    }


    // Create mapper objects.
    vector<Mapper*> mappers;
    for (int i = 0; i < mappers_cnt; i++) {
        Mapper *mapper = new Mapper(i, &reducer_barrier, mappers_result[i],
                                    files, file_mutexes, parsed_file);
        mappers.push_back(mapper);
    }

    // Create reducer objects.
    vector<Reducer*> reducers;
    for (int i = 0; i < reducers_cnt; i++) {
        Reducer *reducer = new Reducer(i, &reducer_barrier, mappers_result);
        reducers.push_back(reducer);
    }

    // Allocate memory for the array of threads.
    pthread_t *threads;
    try {
        threads = new pthread_t[mappers_cnt + reducers_cnt];
    } catch (bad_alloc &exception) {
        fprintf(stderr, "Allocation failed.\n");
        exit(-1);
    }

    cout << "GOT HERE\n";

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


    int checkParsed = false;
    for (int i = 0; i < files.size(); i++) {
        if (!parsed_file[i]) {
            checkParsed = true;
            cout << "File " << i << " not parsed\n";
        }
    }

    if (!checkParsed) {
        cout << "All files parsed.\n";
    }

    // Free resources.
    delete[] threads;

    for (pthread_mutex_t* mutex : file_mutexes) {
        pthread_mutex_destroy(mutex);
    }

    for (long unsigned int i = 0; i < mappers.size(); i++) {
        delete mappers[i];
    }

    for (long unsigned int i = 0; i < reducers.size(); i++) {
        delete reducers[i];
    }

    pthread_barrier_destroy(&reducer_barrier);


    // for (int i = 0; i < mappers_cnt; i++) {
    //     cout << "Results of mapper_" << i << ": ";
    //     cout << "mapper_" << i << "[tudor] = " << mappers_result[i]["tudor"] << "\n";
    // }



    return 0;
}
