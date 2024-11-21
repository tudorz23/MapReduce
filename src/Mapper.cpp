#include "Mapper.h"

#include <iostream>
#include <fstream>

using namespace std;


Mapper::Mapper(int id, pthread_barrier_t *reducer_barrier,
                std::map<std::string, std::set<int>> &result,
                std::vector<std::string> &files,
                std::vector<pthread_mutex_t*> &file_mutexes,
                std::vector<int> &parsed_file):
                result(result), files(files), file_mutexes(file_mutexes),
                parsed_file(parsed_file)
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
        // Atomically check if the file was already processed.
        bool should_parse = false;

        pthread_mutex_lock(file_mutexes[i]);
        if (parsed_file[i] == 0) {
            should_parse = true;
            parsed_file[i]++;
        }
        pthread_mutex_unlock(file_mutexes[i]);

        if (!should_parse) {
            continue;
        }

        // Parse the file.
        parse_file(i);
        parsed_cnt++;
    }

    // Signal that the work of this mapper is finished.
    // printf("Mapper %d ---- parsed %d files.\n", id, parsed_cnt);
    pthread_barrier_wait(reducer_barrier);
}


void Mapper::parse_file(const int index) {
    ifstream fin(files[index]);

    string word;
    const int real_index = index + 1;

    while (fin >> word) {
        // Remove unwanted characters (only keep letters).
        string::iterator it = word.begin();

        while (it != word.end()) {
            if (!(('A' <= *it && *it <= 'Z') || ('a' <= *it && *it <= 'z'))) {
                it = word.erase(it);
                continue;
            }

            // Set the character to lowercase.
            if (*it < 'a') {
                *it += 32;
            }

            it++;
        }

        // Add the word only if it is not empty.
        if (!word.empty()) {
            result[word].insert(real_index);
        }
    }

    fin.close();
}
