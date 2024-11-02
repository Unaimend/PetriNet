#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <iostream>

struct Config {
  Config() = default;
  Config(int threads, int iterations) :
    threads{threads}, iterations{iterations} {}
  int threads;
  int iterations;
};


Config loadConfig(const std::filesystem::path& p);
int run(int, char**, const Config& config);
// TODO Should take iteraratos o we avoid copying the StoppingCriterion
double mean(const std::vector<double>& x);
double varianceOpt(const std::vector<double>& x, double mean);
double stdDevOpt(const std::vector<double>& x, double mean);


template <typename T>
void printVector(const std::vector<T>& vec) {
    std::cout << "Vector elements: ";
    for (const T& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

template <typename K, typename V>
void printUnorderedMap(const std::unordered_map<K, V>& umap) {
    std::cout << "Unordered Map elements: " << std::endl;
    for (const auto& pair : umap) {
        std::cout << pair.first << " : " << pair.second.getLabel() << std::endl;
    }
}


#endif
