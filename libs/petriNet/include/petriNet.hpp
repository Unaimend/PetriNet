#ifndef PETRINET_HPP
#define PETRINET_HPP
#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include <filesystem>
void helloFromLib();
namespace petrinet { 
using ID = std::size_t;

// TODO Those could be made into the same type
// then we could just look up the ids in the arc in one map
class Place {
  ID id;
  std::string label;
  std::vector<ID> arcs;
};

class Transition {
  ID id;
  std::string label;
  std::vector<ID> arcs;
};

class Arc {
  ID id;
  ID startID;
  ID endID;
  std::size_t edgeWeight;
};

class PetriNet
{

public:
  void loadFromJSON(const std::filesystem::path& path);


private: 
  std::map<std::size_t, Place> places;
  std::map<std::size_t, Transition> transitions;
  std::map<std::size_t, Arc> arcs;
 
};
}
#endif

