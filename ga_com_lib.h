#ifndef GA_LIBRARY_COMPACT_ALG_GA_LIB_COMPACT_H
#define GA_LIBRARY_COMPACT_ALG_GA_LIB_COMPACT_H

#endif //GA_LIBRARY_COMPACT_ALG_GA_LIB_COMPACT_H

#include <iostream>
#include <ctime>
#include <math.h>
#include <thread>
#include <mutex>

using namespace std;

// Just function that returns result of the formula:
double formula(double x) {
    return 10.0 + x * sin(x);
}

// Generate starting array of probabilities - 0.5 each:
void starting_array(double* probabilities, int length, int current_thread, int num_of_threads) {
    if (current_thread != num_of_threads) {
        for (int i = (length / num_of_threads) * (current_thread - 1);
             i < (length / num_of_threads) * (current_thread); i++) {
            probabilities[i] = 0.5;
        }
    }
    else{
        for (int i = (length / num_of_threads) * (current_thread - 1);
             i < length; i++) {
            probabilities[i] = 0.5;
        }
    }
}

// Return binary digit of creature:
int get_bit(unsigned long long creature, int index) {
    return (creature >> index) & 1;
}

// Return creature with replaced binary digit in given position:
long long put_bit(unsigned long long creature, int index, int bit) {
    return (creature & ~(1 << index)) | (bit << index);
}

// Return string representation of the creature:
string creature_to_string(unsigned long long creature, int length, int number_of_threads, int current_thread) {
    string creature_string;

    if (number_of_threads != current_thread) {
        for (int i = (length - 1)/number_of_threads * (current_thread - 1); i >= (length - 1)/number_of_threads * (current_thread); i--) {
            creature_string += (get_bit(creature, i) + 48);
        }
    }
    else{
        for (int i = (length - 1)/number_of_threads * (current_thread - 1); i >= 0; i--) {
            creature_string += (get_bit(creature, i) + 48);
        }
    }
    return creature_string;
}

// Print creature:
void print_creature(unsigned long long creature, int length) {
    string repr[4];
    int num_of_threads = 4;
    auto *new_threads_arr1 = new thread[num_of_threads];
    for(int i = 0; i<num_of_threads; i++){
        new_threads_arr1[i] = thread{[&]{repr[i] = creature_to_string(creature, length, i+1, num_of_threads);}};
    }
    for(int i = 0; i<num_of_threads; i++){
        if(new_threads_arr1[i].joinable()){
            new_threads_arr1[i].join();
        }
    }
    for (int i = 3; i >= 0; i++){
        cout<<repr[i];
    }
    cout<<endl;
}

// Return random double floating point number:
double rand_key() {
    return ((double)rand()) / RAND_MAX;
}
// Generating creature based on probability array:
unsigned long long new_creature(double* probabilities, int length) {
    unsigned long long creature = 0;
    for(int i = 0; i < length; i++) {
        int bit = 0;
        if(rand_key() <= probabilities[i]) bit = 1;
        creature = put_bit(creature, i, bit);
    }

    return creature;
}

// Transform creature into the point on x axis:
double creature_to_x(unsigned long long creature, int length, double x_length) {
    return x_length * (creature / pow(2, length));
}

// Calculate fitness of the creature:
double fitness(unsigned long long creature, int length) {
    return formula(creature_to_x(creature, length, 10.0));
}

// Return 1 if number is close enough to 1, return 0 if number is close enough to 0,
// return -1 otherwise:
int probability_value_is_ready(double value, int population) {
    if( abs(value - 1) < 1/(10*(double)population) ) return 1;
    else if( abs(value) < 1/(10*(double)population) ) return 0;
    else return -1;
}

// Return 1 if array is not ready, return 0 otherwise:
int array_is_not_ready(double* probabilities, int length, int population) {
    for(int i = 0; i < length; i++) {
        if(probability_value_is_ready(probabilities[i], population) == -1) return 1;
    }
    return 0;
}

int run_genetic_algorithm(int length, int population) {
    mutex mtx;
    int num_of_threads = 4;
    // Creating starting array:
    double probabilities[length];
    auto *new_threads_arr = new thread[num_of_threads];
    for(int i = 0; i<num_of_threads; i++){
        new_threads_arr[i] = thread{starting_array, &probabilities[0], length, i+1, num_of_threads};
    }
    for(int i = 0; i<num_of_threads; i++){
        if(new_threads_arr[i].joinable()){
            new_threads_arr[i].join();
        }
    }

    int generation = 0;

    while(array_is_not_ready(probabilities, length, population)) {

        unsigned long long creature_1 = new_creature(probabilities, length);
        unsigned long long creature_2 = new_creature(probabilities, length);
        // Calculating fitness:
        double fitness_1 = fitness(creature_1, length);
        double fitness_2 = fitness(creature_2, length);

        // Making winner and loser by fitness:
        unsigned long long winner;
        unsigned long long loser;
        double winner_fitness;
        double loser_fitness;
        if(fitness_1 > fitness_2) {
            winner = creature_1;
            winner_fitness = fitness_1;
            loser = creature_2;
            loser_fitness = fitness_2;
        } else {
            winner = creature_2;
            winner_fitness = fitness_2;
            loser = creature_1;
            loser_fitness = fitness_1;
        }

        // Changing probability array:
        for(int i = 0; i < length; i++) {
            if(get_bit(winner, i) != get_bit(loser, i)) {
                if(get_bit(winner, i) == 1) {
                    probabilities[i] = probabilities[i] + 1/((double)population);
                } else {
                    probabilities[i] = probabilities[i] - 1/((double)population);
                }
            }
        }

        generation++;
        cout << generation << ") " << "Best fitness: " << winner_fitness << endl;

    }

    return 0;
}
