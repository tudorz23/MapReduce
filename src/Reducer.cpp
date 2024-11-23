#include "Reducer.h"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;


Reducer::Reducer(int id, char start_char, char end_char, pthread_barrier_t *reducer_barrier,
                 std::vector<std::map<std::string, std::set<int>>> &mappers_result):
                    mappers_result(mappers_result)
{
    this->id = id;
    this->start_char = start_char;
    this->end_char = end_char;
    this->reducer_barrier = reducer_barrier;
    this->mappers_result = mappers_result;
}


Reducer::~Reducer() {
}


void Reducer::execute_reduce() {
    // Wait until all the mappers finish their work.
    pthread_barrier_wait(reducer_barrier);

    // Create a map that will contain, for each letter that the reducer is
    // responsible for, all the words that begin with that letter and
    // for each word, a set with the files it is present in.
    map<char, map<string, set<int>>> words_by_char;

    // Iterate each mapper result.
    for (const auto &m_result : mappers_result) {
        // Iterate all the words that were mapped by the mapper.
        // The ordered map ensures they are sorted alphabetically.
        for (const auto &[word, files] : m_result) {
            if (word[0] < start_char) {
                // Has not reached the target letter interval.
                continue;
            }

            if (word[0] > end_char) {
                // Has passed the target letter interval.
                break;
            }

            // In the target interval, so add the word to the
            // words_by_char map, with all the files in which it is present.
            words_by_char[word[0]][word].insert(files.begin(), files.end());
        }
    }

    // Now, the words_by_char map is fully populated.
    // For every character, sort its words by the number of files they
    // appear in and write to the output file.

    for (char c = start_char; c <= end_char; c++) {
        // Check if c is in the words_by_char map.
        if (words_by_char.find(c) == words_by_char.end()) {
            // Not present, create an empty file.
            ofstream out_file;
            out_file.open(std::string(1, c) + ".txt");

            if (!out_file.is_open()) {
                cerr << "Failed to open out file for char " << c << "\n";
                exit(-1);
            }

            out_file.close();
            continue;
        }

        // Create a vector to be able to sort the words with std::sort().
        vector<pair<string, set<int>>> all_words_with_c;

        for (auto &[word, files] : words_by_char[c]) {
            all_words_with_c.emplace_back(word, files);
        }

        // Sort the words by the size of the set, and then alphabetically.
        sort(all_words_with_c.begin(), all_words_with_c.end(),
            [](const auto &a, const auto &b) {
                if (a.second.size() == b.second.size()) {
                    return a.first < b.first;
                }

                return a.second.size() > b.second.size();
            });

        // Write the results to the file of character c.
        ofstream out_file;
        out_file.open(std::string(1, c) + ".txt");

        if (!out_file.is_open()) {
            cerr << "Failed to open out file for char " << c << "\n";
            exit(-1);
        }

        for (unsigned long i = 0; i < all_words_with_c.size(); i++) {
            auto &[word, files] = all_words_with_c[i];

            out_file << word << ":[";

            set<int>::iterator it = files.begin();

            while (it != files.end()) {
                out_file << *it;

                it++;
                if (it != files.end()) {
                    out_file << " ";
                }
            }

            out_file << "]\n";
        }

        out_file.close();
    }
}
