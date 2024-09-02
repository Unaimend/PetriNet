#include <iostream>
#include <filesystem>
#include "../libs/petriNet/include/petriNet.hpp"

int main(int argc, char **argv) {
  auto filepath = std::filesystem::path{argv[1]};

  petrinet::PetriNet p;
  p.loadFromJSON(filepath);
  return 0;
}
