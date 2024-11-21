//
// Created by Tudor Zaharia on 21/11/2024.
//
#include <string>
#include <iostream>

using namespace std;

int main() {
    string word;
    cin >> word;

    string::iterator it = word.begin();

    // Remove unwanted characters.
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

    cout << word << "\n";
}