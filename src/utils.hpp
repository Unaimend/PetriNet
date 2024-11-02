#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <iostream>

enum class Mode {
  ITERTATION,
  STOPPING_CRITERION
};

struct Config {
  Config() = default;
  Config(int threads, int iterations = 10,
      uint64_t historyLength = 5, double stdDevThresh = 0.5 ) :
    threads{threads}, iterations{iterations},  historyLength{historyLength}, stdDevThresh{stdDevThresh} {}
  int threads{};
  int iterations{};
  uint64_t historyLength{};
  Mode mode = Mode::ITERTATION;
  double stdDevThresh{};

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
