#include <filesystem>
#include "../libs/petriNet/include/petriNet.hpp"
#include <future>
const int THREADS = 1;
const int AMOUNT_OF_RUNS = 50;

petrinet::PetriNet* executeRun(const std::filesystem::path& filepath) {
  auto* p = new petrinet::PetriNet();
  p->loadFromJSON(filepath);
  p->simulateNShuffe(AMOUNT_OF_RUNS);
  return p;
}

int run(int  /*argc*/, char** argv) {
  auto filepath = std::filesystem::path{argv[1]};

  std::vector<std::future<petrinet::PetriNet*>> threads;

    // Launch a group of threads
  for (int i = 0; i < THREADS; ++i) {
    threads.emplace_back(std::async(std::launch::async, executeRun, filepath));
  }

  int i = 1;
  // Join the threads with the main thread
  for (auto& th : threads) {
    auto* p = th.get();
    p->saveFinalTokenCount(std::format("FTC_{}.json", i)); 
    p->saveReactionActivityCount(std::format("RAC_{}.json", i));
    p->saveReactionActivityHistory(std::format("RAH_{}.json", i));
    p->saveTokenHistory(std::format("TH_{}.json", i)); //p.saveTokenHistory("TH.json");
    i++;
  }

  return 0;
}
