#ifndef GA_LIBRARY_GA_LIB_H
#define GA_LIBRARY_GA_LIB_H

#endif //GA_LIBRARY_GA_LIB_H

#include <iostream>
#include <ctime>
#include <math.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>


using namespace std;

// Just function that returns result of the formula:
double formula(double x) {
    return 10.0 + x * sin(x);
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
string creature_to_string(unsigned long long creature, int length) {
    string creature_string;

    for(int i = length - 1; i >= 0; i--) {
        creature_string += (get_bit(creature, i) + 48);
    }

    return creature_string;
}

// Print creature:
void print_creature(unsigned long long creature, int length) {
    cout << creature_to_string(creature, length) << endl;
}

// Return  bit of the creature:
long long invert_bit(unsigned long long creature, int index) {
    int bit = get_bit(creature, index);
    if(bit) bit = 0; else bit = 1;
    return put_bit(creature, index, bit);
}

// Generate array of random integer bits:
int generate_random_bits(int length, int* result) {

    for(int i = 0; i < length; i++) {
        result[i] = rand() % 2;
    }

    return 0;
}

// Transform creature into the point on x axis:
double creature_to_x(unsigned long long creature, int length, double x_length) {
    return x_length * (creature / pow(2, length));
}

// Calculate fitness of the creature:
double fitness(unsigned long long creature, int length) {
    return formula(creature_to_x(creature, length, 10.0));
}

// Do crossingover:
unsigned long long crossingover(unsigned long long creature_1, unsigned long long creature_2,
                                int crossing_point, int length) {
    unsigned long long child = 0;

    for(int i = 0; i < crossing_point; i++) {
        int bit = get_bit(creature_1, i);
        child = put_bit(child, i, bit);
    }

    for(int i = 0; i + crossing_point < length; i++) {
        int bit = get_bit(creature_2, i);
        child = put_bit(child, crossing_point + i, bit);
    }

    return child;
}

// Generate random creature:
unsigned long long random_creature(int length) {
    unsigned long long creature = 0;
    for(int i = 0; i < length; i++) {
        creature = put_bit(creature, i, rand() % 2);
    }
    return creature;
}

void random_population(int population_length, int creature_length, unsigned long long *array) {
    for (int i = 0; i < population_length; i++) {
        array[i] = random_creature(creature_length);
    }

}

// Get n most fit individuals out of the population:
void n_most_fit(int population_length, int creature_length, int n,
                unsigned long long *population, unsigned long long *most_fit_ones) {

    // Creating array with values of fitness of the population and filling it:
    double fitness_values[population_length];
    for(int i = 0; i < population_length; i++) {
        fitness_values[i] = fitness(population[i], creature_length);
    }

    // Finding creature with biggest fitness, replace fitness with -1.0
    // in population_fitness, do this until result is filled:
    for(int i = 0; i < n; i++) {
        int max_fitness_index = 0;
        double max_fitness = fitness_values[0];

        for(int j = 0; j < population_length; j++) {
            if(fitness_values[j] > max_fitness) {
                max_fitness_index = j;
                max_fitness = fitness_values[j];
            }
        }

        most_fit_ones[i] = population[max_fitness_index];
        fitness_values[max_fitness_index] = -1;
    }
}

// Breed parents to get children:
void breed_creatures(int parents_amount, int kids_amount, unsigned long long* parents_array,
                     unsigned long long* kids_array, int creature_length) {
    // Creature can  be bred with itself.
    for(int i = 0; i < kids_amount; i++) {
        // Selecting parents at random:
        unsigned long long parent_1 = parents_array[rand() % (parents_amount)];
        unsigned long long parent_2 = parents_array[rand() % (parents_amount)];
        kids_array[i] = crossingover(parent_1, parent_2,
                                     rand() % (creature_length + 1),
                                     creature_length);
    }
}

unsigned long long mutate_creature(unsigned long long creature, int length) {
    return invert_bit(creature, rand() % length);
}

void mutate_population(unsigned long long* population, int population_length,
                       int creature_length, int one_in_n_mutation_chance) {

    for(int i = 0; i < population_length; i++) {
        if((rand() % one_in_n_mutation_chance) == 0) {
            population[i] = mutate_creature(population[i], creature_length);
        }
    }
}

// Replace population with new generation:
void new_generation(int population_length, int creature_length, unsigned long long* population,
                    int fittest_amount, int one_in_n_mutation_chance) {

    unsigned long long fittest_ones[fittest_amount];
    unsigned long long children[population_length - fittest_amount];

    n_most_fit(population_length, creature_length, fittest_amount, population, fittest_ones);

    breed_creatures(fittest_amount, population_length - fittest_amount, fittest_ones, children,
                    creature_length);

    // Mutating children:
    mutate_population(children, population_length - fittest_amount, creature_length,
                      one_in_n_mutation_chance);

    for(int i = 0; i < fittest_amount; i++) {
        population[i] = fittest_ones[i];
    }

    for(int i = fittest_amount; i < population_length; i++) {
        population[i] = children[i - fittest_amount];
    }
}

double best_fitness(unsigned long long* population, int amount, int length) {
    double max_fitness = fitness(population[0], length);

    for(int i = 0; i < amount; i++) {
        if(fitness(population[i], length) > max_fitness) max_fitness = fitness(population[i], length);
    }
    return max_fitness;
}

double average_fitness(unsigned long long* population, int amount, int length) {
    double sum = 0;
    for(int i = 0; i < amount; i++) {
        sum += fitness(population[i], length);
    }
    return sum/amount;
}

void show_fitness(unsigned long long* population, int amount, int length, int generation) {
    std::cout << generation << ") Average Fitness: " <<
              average_fitness(population, amount, length) <<
              "; Best Fitness: " << best_fitness(population, amount, length) << std::endl;
}

int run_genetic_algorithm(int population_length, int creature_length,
                          int one_in_n_mutation_chance, int parents_amount,
                          int generations_amount) {

    // Generate initial population:
    unsigned long long population[population_length];
    random_population(population_length, creature_length, population);

    // Compute fitness:
    cout << "In the beginning:" << endl;
    show_fitness(population, population_length, creature_length, -1);
    cout << "____________________________________" << endl;

    // For each generation:
    for(int generation = 0; generation < generations_amount; generation++) {
        // Selection and Crossover:
        new_generation(population_length, creature_length, population, parents_amount,
                       one_in_n_mutation_chance);

//        // Mutation:
//        mutate_population(population, population_length, creature_length,
//                          one_in_n_mutation_chance);

        // Compute fitness:
        show_fitness(population, population_length, creature_length, generation);
    }

    return 0;
}
