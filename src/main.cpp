#include <iostream>
#include <filesystem>
#include "../libs/petriNet/include/petriNet.hpp"
#include <thread>

int main(int argc, char **argv) {
  auto filepath = std::filesystem::path{argv[1]};

  petrinet::PetriNet p;
  p.loadFromJSON(filepath);

  p.simulateNShuffe(1'000'000);
  //p.toDot("test.dot");
  p.saveFinalTokenCount("FTC.json");
  //p.saveTokenHistory("TH.json");
  //p.saveReactionActivity("RA.json");
  return 0;
}
