#ifndef PETRINET_HPP
#define PETRINET_HPP
#include <cstddef>
#include <map>
#include <iostream>
#include <ranges>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <print>
void helloFromLib();
namespace petrinet { 
using ID = std::size_t;

// TODO Those could be made into the same type
// then we could just look up the ids in the arc in one map
//
//Reason for std::move https://old.reddit.com/r/cpp_questions/comments/lzqc3j/passing_rvalue_reference_forward/
class Place {
public:
// TODO Maybe take as lvalue and moce (i.e. no &&)
  Place(ID id, std::string&& label, std::vector<ID>&& arcs, std::size_t tokens) : 
    id{id}, label{label}, arcs{std::move(arcs)}, tokens{tokens} {
  }
  Place(const Place&) = delete;
  Place& operator=(const Place&) = delete;
//  TODO Check what the default move constructor does
  Place(Place&&) = default;
  Place& operator=(Place&&) = default;
  ~Place() = default;

  const std::string& getLabel() const {
    return label;
  }

  const std::size_t& getTokens() const {
    return tokens;
  }

  void setTokens(std::size_t token) {
    tokens = token;
  }

private:
  ID id;
  std::string label;
  std::vector<ID> arcs;
  std::size_t tokens;

};


struct Arc {
  ID id;
  ID startID;
  ID endID;
  std::size_t edgeWeight;
};


class Transition {
public:
  // TODO Maybe take as lvalue and moce
  Transition(ID id, std::string&& label, std::vector<ID>&& arcs) : id{id}, label{label}, arcs{std::move(arcs)} {
  }
  Transition(const Transition&) = delete;
  Transition& operator=(const Transition &) = delete;
//  TODO Check what the default move constructor does
  Transition(Transition&&) = default;
  Transition& operator=(Transition&&) = default;
  ~Transition() = default;

  const std::string& getLabel() const {
    return label;
  }

private:
  ID id;
  std::string label;
  std::vector<ID> arcs;
};

class PetriNet
{

public:
  void loadFromJSON(const std::filesystem::path& path);

// TODO Maybe take as lvalue and moce
  void addTransition(ID id, Transition&& transition) {
    transitions.insert_or_assign(id, std::move(transition));
  }


  void addPlace(ID id, Place&& place) {
    places.insert_or_assign(id, std::move(place));
  }


  void addArc(ID id, const Arc& arc) {
    arcs.insert_or_assign(id, arc);
  }

  void toDot(const std::filesystem::path& path);


  inline void simulateNShuffe(int N) {
    while((N--) != 0) {
      simulateSingleGradient();
    }

  }

  inline void simulateSingleGradient() {
    // Vector to store the keys
    // TODO Preallocate the right capacity
    std::vector<std::size_t> keys;

    // Iterate over the map and store keys
    for (const auto& pair : transitions) {
        keys.push_back(pair.first);
    }

    std::ranges::shuffle(keys, gen);

    for (const auto& id : keys) {
      auto outGoingArcs = getOutGoingArcs(id);
      auto inComingArcs = getInComingArcs(id);


      // TODO Preallocate the right capacity
      // TODO Move the code below into own function
      auto outgoingTokens = std::vector<std::size_t>{};
      auto incomingTokens = std::vector<std::size_t>{};
      for(const Arc& arc: outGoingArcs) {

        std::println("Arc {} is leaving from {}",arc.id, id);
        // Because we are iteratong over transitions and looking at arcs that 
        // leave me I know that arc.endID refers to a place
        auto l = places.at(arc.endID).getLabel();
        auto t = places.at(arc.endID).getTokens();
        std::println("{} has beed added to the outgoing tokens of {} with {} tokens", l, id, t);
        outgoingTokens.push_back(t);
      }
      
      for(const Arc& arc: inComingArcs) {
        std::println("Arc {} coming into {}",arc.id, id);
        // Because we are iteratong over transitions and looking at arcs that 
        // coming in to me I know that arc.startID refers to a place
        auto l = places.at(arc.startID).getLabel();
        auto t = places.at(arc.startID).getTokens();
        std::println("{} has beed added to the incoming tokens of {} with {} tokens", l, id, t);
        incomingTokens.push_back(t);
      }
      //TODO I dont need the min I just hae to check that 0 is not in there
      auto minIncToken = std::min_element(incomingTokens.begin(), incomingTokens.end());
      auto outSum = std::accumulate(outgoingTokens.begin(), outgoingTokens.end(), static_cast<std::size_t>(0));
      auto incSum = std::accumulate(incomingTokens.begin(), incomingTokens.end(), static_cast<std::size_t>(0));


      std::cout << *minIncToken << " " << incSum << " " << outSum << "\n";

      if(*minIncToken > 0 && incSum > outSum) {
        std::println("{} has fired", id);
        for(const Arc& arc: outGoingArcs) {
          auto t = places.at(arc.endID).getTokens();
          places.at(arc.endID).setTokens(t+1);
        }

        for(const Arc& arc: inComingArcs) {
          auto t = places.at(arc.startID).getTokens();
          places.at(arc.startID).setTokens(t -1);
        }

      }
    }
  }


  std::vector<Arc> getInComingArcs(ID id_) {
    std::vector<Arc> inComingArcs;
    // TODO Get rid of code duplication
    //Figure out if its a place or a transition
    if(auto it = places.find(id_); it != places.end()) {
      for(const auto& [id, arc] : arcs) {
        //TODO Mayube us std::refs to not copy here?
        if(arc.endID == id_) {
          inComingArcs.push_back(arc);
        }
      }
    }

    if(auto it = transitions.find(id_); it != transitions.end()) {
      for(const auto& [id, arc] : arcs) {
        //TODO Mayube us std::refs to not copy here?
        if(arc.endID == id_) {
          inComingArcs.push_back(arc);
        }
      }
    }
    return inComingArcs;
  }


  std::vector<Arc> getOutGoingArcs(ID id_) {
    std::vector<Arc> outGoingArcs;
    // TODO Get rid of code duplication
    //Figure out if its a place or a transition
    if(auto it = places.find(id_); it != places.end()) {
      for(const auto& [id, arc] : arcs) {
        //TODO Mayube us std::refs to not copy here?
        if(arc.startID == id_) {
          outGoingArcs.push_back(arc);
        }
      }
    }

    if(auto it = transitions.find(id_); it != transitions.end()) {
      for(const auto& [id, arc] : arcs) {
        //TODO Mayube us std::refs to not copy here?
        if(arc.startID == id_) {
          outGoingArcs.push_back(arc);
        }
      }
    }
    return outGoingArcs;
  }

private: 
  //TODO Make one map that contains transition and place that. 
  //Each elements carreis is typer as an enum
  std::map<ID, Place> places;
  std::map<ID, Transition> transitions;
  std::map<ID, Arc> arcs;
  std::random_device rd;
  std::mt19937 gen {rd()};
 
 
};
}
#endif

