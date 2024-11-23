#include <iostream>
#include <pthread.h>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include "Mapper.h"
#include "Reducer.h"

#define NR_OF_LETTERS 26

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
    ifstream arg_file_in;
    arg_file_in.open(user_file);

    if (!arg_file_in.is_open()) {
        cerr << "Failed to open the command-line file.\n";
        exit(-1);
    }

    int entries;
    arg_file_in >> entries;

    string line;

    for (int i = 0; i < entries; i++) {
        arg_file_in >> line;
        files.push_back(line);
    }

    arg_file_in.close();
}


// TODO: Check if memory allocations succeeded.
int main(int argc, char **argv) {
    // Get the number of mappers and the number of reducers.
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <MAPPERS_CNT> <REDUCERS_CNT> <INPUT_FILE>\n";
        exit(-1);
    }

    // Return code used for every function return code check.
    int r;

    int mappers_cnt;
    r = sscanf(argv[1], "%d", &mappers_cnt);
    if (r != 1) {
        cerr << "Invalid number of mappers.\n";
        exit(-1);
    }

    int reducers_cnt;
    r = sscanf(argv[2], "%d", &reducers_cnt);
    if (r != 1) {
        cerr << "Invalid number of reducers.\n";
        exit(-1);
    }

    // Get the input files.
    vector<string> files;
    get_input_files(argv[3], files);


    // Create one mutex for each file to dynamically split them among mappers.
    vector<pthread_mutex_t*> file_mutexes;
    for (unsigned long i = 0; i < files.size(); i++) {
        pthread_mutex_t *mutex = new pthread_mutex_t;
        pthread_mutex_init(mutex, NULL);
        file_mutexes.push_back(mutex);
    }

    // Flags to know if a file has already been parsed.
    vector<int> parsed_file(files.size(), 0);


    // Barrier that doesn't allow reducers to start before all the mappers finish.
    pthread_barrier_t reducer_barrier;
    pthread_barrier_init(&reducer_barrier, NULL, mappers_cnt + reducers_cnt);


    // Create the mappers result array, where each mapper will place its result.
    vector<map<string, set<int>>> mappers_result;
    for (int i = 0; i < mappers_cnt; i++) {
        map<string, set<int>> result;
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
    const int proportion = NR_OF_LETTERS / reducers_cnt;

    for (int i = 0; i < reducers_cnt; i++) {
        int start = i * proportion;
        int end;

        if (i == reducers_cnt - 1) {
            end = NR_OF_LETTERS - 1;
        } else {
            end = (i + 1) * proportion - 1;
        }

        Reducer *reducer = new Reducer(i, 'a' + start, 'a' + end,
                                        &reducer_barrier, mappers_result);
        reducers.push_back(reducer);
    }

    // Allocate memory for the array of threads.
    pthread_t *threads = new pthread_t[mappers_cnt + reducers_cnt];

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
            cerr << "Error creating thread " << i << "\n";
            exit(-1);
        }
    }

    // Join the worker threads.
    for (int i = 0; i < mappers_cnt + reducers_cnt; i++) {
        r = pthread_join(threads[i], NULL);

        if (r) {
            cerr << "Error waiting for thread " << i << "\n";
            exit(-1);
        }
    }


    // Free manually allocated resources.
    delete[] threads;

    for (Mapper *mapper : mappers) {
        delete mapper;
    }

    for (Reducer *reducer : reducers) {
        delete reducer;
    }

    pthread_barrier_destroy(&reducer_barrier);

    for (pthread_mutex_t* mutex : file_mutexes) {
        pthread_mutex_destroy(mutex);
        delete mutex;
    }

    return 0;
}
