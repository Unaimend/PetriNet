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
public:
  Place(ID id, std::string&& label, std::vector<ID>&& arcs, std::size_t tokens) : 
    id(id), label(label), arcs(arcs), tokens(tokens) {
  }
  Place(const Place&) = delete;
//  TODO Check what the default move constructor does
  Place(Place&&) = default;
  Place& operator=(Place&&) = default;

private:
  ID id;
  std::string label;
  std::vector<ID> arcs;
  std::size_t tokens;

};

class Transition {
public:
  Transition(ID id, std::string&& label, std::vector<ID>&& arcs) : id(id), label(label), arcs(arcs) {
  }
  Transition(const Transition&) = delete;
//  TODO Check what the default move constructor does
  Transition(Transition&&) = default;
  Transition& operator=(Transition&&) = default;

private:
  ID id;
  std::string label;
  std::vector<ID> arcs;
};

struct Arc {
  ID id;
  ID startID;
  ID endID;
  std::size_t edgeWeight;
};

class PetriNet
{

public:
  void loadFromJSON(const std::filesystem::path& path);
  //void addTransition(ID id, const Transition& transition) {
  //  transitions.insert_or_assign(id, transition);
  //}


  void addTransition(ID id, Transition&& transition) {
    transitions.insert_or_assign(id, std::move(transition));
  }


  void addPlace(ID id, Place&& place) {
    places.insert_or_assign(id, std::move(place));
  }


  void addArc(ID id, const Arc& arc) {
    arcs.insert_or_assign(id, arc);
  }

private: 
  std::map<ID, Place> places;
  std::map<ID, Transition> transitions;
  std::map<ID, Arc> arcs;
 
};
}
#endif

