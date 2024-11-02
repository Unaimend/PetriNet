#include <filesystem>
#include "../libs/petriNet/include/petriNet.hpp"
#include <future>
#include "utils.hpp"

static petrinet::PetriNet* executeRun(const std::filesystem::path& filepath, const Config& c) {
  auto* p = new petrinet::PetriNet();
  p->loadFromJSON(filepath);
  if(c.mode == Mode::ITERTATION) {
    p->simulateNShuffe(c.iterations);
  } else if (c.mode == Mode::STOPPING_CRITERION) {
    p->simulateStoppingCritertion(c.historyLength, c.iterations, c.stdDevThresh);
  }
  return p;
}

int run(int  /*argc*/, char** argv, const Config& config) {
  auto filepath = std::filesystem::path{argv[1]};

  std::vector<std::future<petrinet::PetriNet*>> threads;

    // Launch a group of threads
  for (int i = 0; i < config.threads; ++i) {
    threads.emplace_back(std::async(std::launch::async, executeRun, filepath, config));
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
