#include <iostream>
#include <filesystem>
#include "../libs/petriNet/include/petriNet.hpp"
#include <thread>

int run(int argc, char** argv) {
  auto filepath = std::filesystem::path{argv[1]};

  petrinet::PetriNet p;
  p.loadFromJSON(filepath);

  p.simulateNShuffe(100);
  //p.toDot("test.dot");
  p.saveFinalTokenCount("FTC.json");
  p.saveTokenHistory("TH.json");
  p.saveReactionActivityCount("RAC.json");
  p.saveReactionActivityHistory("RAH.json");
  p.saveBlockedByCount("BBC.json");
  return 0;
}
