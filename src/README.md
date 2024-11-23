*Designed by Marius-Tudor Zaharia, 333CA, November 2024*

# Inverted Index using Map-Reduce

---

## Table of contents
1. [Overview](#overview)
2. [File distribution](#file-distribution)
3. [Running](#running)
4. [Implementation details](#implementation-details)
    * [Main thread logic](#main-thread-logic)
    * [Mapper logic](#mapper-logic)
    * [Reducer logic](#reducer-logic)
5. [Final thoughts](#final-thoughts)

---

## Overview
* This is an implementation of a parallel algorithm that computes an inverted
index for a set of input files, following the Map-Reduce paradigm.
* It is written in C++, using the `pthreads` library for multithreading.
* The input files are split between multiple `Mapper` threads, with them 
building partial lists that contain mappings of `words` and the `ID of the
files` in which they can be found.
* Next, the results are aggregated by `Reducer` threads, creating, for every
letter of the alphabet, an output file containing all the words that begin with
that letter and for every word, the files it appears in. The words are sorted
by the number of files, and then alphabetically.

---

## File distribution
* The `src` directory contains the source code.
* The logic of the `Mapper` threads is implemented by the `Mapper` class, while
the `Reducer` class presents the logic of the `Reducer` threads, thus allowing
a good modularization of the code.

---

## Running
* To compile the project, `make` should be run from `src`, generating an
executable called `tema1`.
* The running command is `./tema1 <MAPPERS_CNT> <REDUCERS_CNT> <INPUT_FILE>`.
* The `checker` directory contains an automated testing script.

---

## Implementation details
### Main thread logic
* The main thread parses the input file and stores the target file names, which
will be passed to the `Mapper` threads.
* It allocates all the necessary resources, including those that will be shared
among threads. It prepares a vector of `mappers results`, and passes to each
mapper a reference only to the element of this vector that it will be
responsible to write in. The reducers get access to the whole vector, as they
are responsible with the aggregation of the results.
* It then instantiates the necessary number of mapper and reducer objects,
based on the command line input. The threads are then created in a single `for`
loop, the first `MAPPERS_CNT` being mappers, and the last `REDUCERS_CNT` being
reducers.
* To make sure the reducers only begin their work after all the mappers have
finished theirs, a `barrier` is used. The number of threads required for the
barrier to allow the passing is `MAPPERS_CNT + REDUCERS_CNT`. Thus, all the
reducers will immediately wait at the barrier, while the mappers reach the
barrier only after all their work is done, ensuring thread synchronization.
* The threads are also all `joined` in a single `for` loop, ending the
algorithm. The main thread then frees all the allocated resources.


### Mapper logic
* A dynamic way of splitting the files among mappers was chosen, thus ensuring
that even if one mapper is stuck parsing a big file, another one can continue
to parse the following files, that would have otherwise been assigned to it.
* Thus, each mapper begins to parse the file with the `MAPPER_ID` index in the
files vector. A vector of `bools` is used to store the state of the files:
`already parsed` or `not parsed`. As this vector is shared among all the mappers,
race conditions can occur. Not to lock the whole vector, a vector of `mutex` is
used instead, with each mutex being responsible for one file, mimicking the
`synchronized(parsed_file[i])` functionality of Java. An additional variable,
`should_parse`, is used so the locked region is as small as possible.
* Each mapper iterates over all the file indexes from the vector, starting from
its own id, and checks if the file should be parsed or not. Thus, the work will
be evenly shared among the mapper threads.
* For the file parsing part, the file is opened and all the words from it are
scanned and modified, removing characters that are not letters. The words that
are not empty after this operation are added in the mapper's own `result map`,
an entry in the `mappers results` vector earlier discussed.
* This map stores words as the key and a set of integers as values,
representing the indexes of the files (parsed by the current mapper) that
contain that word.
* A `std::map` is used, so an ordered one, to store the words in an alphabetic
order, which will make sense for the Reducer logic. From what I read, the
performance hit of using a `map` instead of an `unordered_map` should not be
very big for `string` keys, because when comparing them, only the first few
characters are traversed, until they do not match anymore, while for the
unordered one, a hash is calculated, so the whole string is traversed anyway.


### Reducer Logic
* Each reducer will have an interval of letters to manage, pre-defined and
passed to them from the main thread. Thus, every reducer will store a local
`map`, with the letter as key and another map as value, that will contain all
the words starting with that letter.
* The internal map is from strings to `set<int>`, with the same meaning as
the one from the mappers. This time however, it will contain all the file
indexes for the word, not just the files parsed by one mapper.
* To populate this big map, a reducer iterates the `mappers results` vector,
so the results of all the mappers, but will only consider those words that
start with one of the letters it is responsible for. This is where the fact
that the maps from this vector are ordered comes into play: a reducer can break
out of the loop for one mapper result when it reaches a word with the starting
letter lexicographically bigger than the last letter the reducer takes care of.
* It can be argued that the last reducer (i.e. the one that manages the last
letters of the alphabet) will iterate the whole maps anyway. While that is
correct, there are fewer words starting with many of the letters this reducer
is responsible for (i.e. u, v, x, y, z), so the time lost here is compensated
in the later stages, where this reducer will have fewer data to manage.
* After populating the `words_by_char` map, the reducer has to sort the words
that start with the same letter. To do that, for every letter it manages, it
populates a vector of `pair<string, set<int>>` with words and their files.
This vector can easily be sorted using a lambda expression comparator passed to
`std::sort()`.
* It then writes the results of the inverted index in one file for each letter.

---

## Final thoughts
* It was an interesting project, from which I learnt about the Map-Reduce
paradigm, and also new things about C++, such as storing reference types as
class attributes and the need to initialise them with `initializer lists`.
* The program was tested against memory leaks using `valgrind`.
* `map` vs `unordered_map`:
https://stackoverflow.com/questions/2196995/is-there-any-advantage-of-using-map-over-unordered-map-in-case-of-trivial-keys
