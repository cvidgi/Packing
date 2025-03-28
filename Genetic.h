#include <set>
#include <unordered_map>
#include <random>
#include <iostream>
#include "Helpers.h"

void Mutation(std::vector<Gen> &data, unsigned int count1, unsigned int count2);

std::vector<Gen> Crossing(std::vector<Gen> first, std::vector<Gen> second);

std::vector<std::vector<Gen>> GeneratePopulation(std::vector<Node> &data, int count);

void PlaceFirstType(Gen &gen, int W, int L, int H, bool check); // Place closer to corner (0, 0, 0)

void PlaceSecondType(Gen &gen, int W, int L, int H, bool check); // Place closer to corner (0, W, L)

void CheckCorrectness(unsigned int size); // Checking the correctness of the arrangement

ld CalcPercGen(std::vector<Gen> genotype, std::vector<Node> &data, int W, int L); // Arrange the boxes for the gene
// and calculate the percorellation coefficient

std::pair<ld, std::vector<Corner>> Genetic(std::vector<Node> data, int W, int L, int generations,
           int count = 100, int cross_count = 20, int pop_limit = 15,
           int mutation_count = 10);
