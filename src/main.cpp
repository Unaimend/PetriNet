#include "utils.hpp"


int main(int argc, char **argv) {
  auto p = std::filesystem::path{"config.json"};
  Config c = loadConfig(p);
  return run(argc, argv, c);

}
