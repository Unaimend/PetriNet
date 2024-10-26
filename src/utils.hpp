#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>

struct Config {
  Config() = default;
  Config(int threads, int iterations) :
    threads{threads}, iterations{iterations} {}
  int threads;
  int iterations;
};


Config loadConfig(const std::filesystem::path& p);
int run(int, char**, const Config& config);
#endif
