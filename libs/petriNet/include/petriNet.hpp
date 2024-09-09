#ifndef PETRINET_HPP
#define PETRINET_HPP
#include <cstddef>
#include <iterator>
#include <map>
#include <unordered_map>
#include "../include/json.hpp"
#include <iostream>
#include <ranges>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <print>

#define METRICS
#define TOKEN_HISTORY
#define REACTION_ACTIVITY_HISTORY
#define REACTION_ACTIVITY_COUNT

#ifdef DEBUG
#define D(x) x
#else
#define D(x) 
#endif


#ifdef METRICS
#define M(x) x
#else
#define M(x)
#endif


#ifdef TOKEN_HISTORY
#define TH(...) __VA_ARGS__
#else
#define TH(x) 
#endif


#ifdef REACTION_ACTIVITY_COUNT
#define RAC(...) __VA_ARGS__
#else
#define RAC(x) 
#endif


#ifdef REACTION_ACTIVITY_HISTORY
#define RAH(...) __VA_ARGS__
#else
#define RAH(x) 
#endif



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
  Place(ID id, std::string&& label, std::vector<ID>&& arcs, std::size_t tokens) noexcept : 
    id{id}, label{label}, arcs{std::move(arcs)}, tokens{tokens}   {
  }

#ifdef METRICS
  explicit Place(const Place& that) : id{that.id}, label{that.label}, arcs{that.arcs}, tokens{that.tokens} {
    // TODO CHeck that this is is not used in high-performance mode aka when metrics are turned off
  }
#endif

  Place& operator=(const Place&) = delete;
//  TODO Check what the default move constructor does
  Place(Place&&) = default;
  Place& operator=(Place&&) = default;
  ~Place() = default;

  [[nodiscard]] const std::string& getLabel() const noexcept{
    return label;
  }

  [[nodiscard]] const std::size_t& getTokens() const noexcept {
    return tokens;
  }

  [[nodiscard]] const ID&  getID() const noexcept {
    return id;
  }

  [[nodiscard]] const std::vector<ID>&  getArcs() const noexcept {
    return arcs;
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
  Transition(ID id, std::string&& label, std::vector<ID>&& arcs) noexcept : id{id}, label{label}, arcs{std::move(arcs)}  {
  }
  Transition(const Transition&) = delete;
  Transition& operator=(const Transition &) = delete;
//  TODO Check what the default move constructor does
  Transition(Transition&&) = default;
  Transition& operator=(Transition&&) = default;
  ~Transition() = default;

  //TODO implement the && versions
  //std::string getName() && { // when we no longer need the value return std::move(name); // we steal and return by value 
  //}
  
  // This is prolblematic when being called on an temporary object(c++ move semantic p. 81) 
  [[nodiscard]] const std::string& getLabel() const noexcept {
    return label;
  }

  // This is prolblematic when being called on an temporary object(c++ move semantic p. 81) 
  [[nodiscard]] const ID&  getID() const noexcept {
    return id;
  }
  // This is prolblematic when being called on an temporary object(c++ move semantic p. 81) 
  [[nodiscard]] const std::vector<ID>&  getArcs() const noexcept {
    return arcs;
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

  //Small object no move necessary
  void addArc(ID id, const Arc& arc) {
    arcs.insert_or_assign(id, arc);
  }

  void toDot(const std::filesystem::path& path);


  inline void simulateNShuffe(int N) {
    while((N--) != 0) {
      simulateSingleGradient();
#if defined(METRICS) 
#ifdef TOKEN_HISTORY
      for(auto& [id, place] : places) {
        tokenHistory[place.getLabel()].push_back(place.getTokens());
      }
#endif
#endif
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
      // Go over each possible transition and
      // collect outgoing and incoming arcs for that id
      auto outGoingArcs = getOutGoingArcs(id);
      auto inComingArcs = getInComingArcs(id);


      // TODO Preallocate the right capacity
      // Get the tokens from incoming and outgoing arcs of `id`
      // We need to know if the the reaction is allowed to fire
      auto outgoingTokens = getOutGoingTokens(outGoingArcs);
      auto incomingTokens = getIncomingTokens(inComingArcs);
      //TODO I dont need the min I just hae to check that 0 is not in there
      auto minIncToken = std::min_element(incomingTokens.begin(), incomingTokens.end());
      auto outSum = std::accumulate(outgoingTokens.begin(), outgoingTokens.end(), static_cast<std::size_t>(0));
      auto incSum = std::accumulate(incomingTokens.begin(), incomingTokens.end(), static_cast<std::size_t>(0));


      D(std::cout << *minIncToken << " " << incSum << " " << outSum << "\n";)
      if(*minIncToken > 0 && incSum > outSum) {
        M(const auto label = transitions.at(id).getLabel();)
        RAC(reactionActivity[label]++;)
        RAH(reactionActivityHistory[label].push_back(true);)
        D(std::println("{} has fired", id);)
        for(const Arc& arc: outGoingArcs) {
          auto t = places.at(arc.endID).getTokens();
          places.at(arc.endID).setTokens(t+1);
        }

        for(const Arc& arc: inComingArcs) {
          auto t = places.at(arc.startID).getTokens();
          places.at(arc.startID).setTokens(t -1);
        }
      } else {
        RAH(reactionActivityHistory[transitions.at(id).getLabel()].push_back(false);)
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


  std::vector<std::size_t> getOutGoingTokens(const std::vector<Arc>& outGoingArcs) {
    auto outgoingTokens = std::vector<std::size_t>{};
    for(const Arc& arc: outGoingArcs) {

      D(std::println("Arc {} is leaving from {}",arc.id, id);)
      // Because we are iteratong over transitions and looking at arcs that 
      // leave me I know that arc.endID refers to a place
      auto l = places.at(arc.endID).getLabel();
      auto t = places.at(arc.endID).getTokens();
      D(std::println("{} has beed added to the outgoing tokens of {} with {} tokens", l, id, t);)
      outgoingTokens.push_back(t);
    }
    // NRVO is madated by C++17
    return outgoingTokens;
  }


  std::vector<std::size_t> getIncomingTokens(const std::vector<Arc>& inComingArcs) {
    auto incomingTokens = std::vector<std::size_t>{};
    for(const Arc& arc: inComingArcs) {
      D(std::println("Arc {} coming into {}",arc.id, id);)
      // Because we are iteratong over transitions and looking at arcs that 
      // coming in to me I know that arc.startID refers to a place
      auto l = places.at(arc.startID).getLabel();
      auto t = places.at(arc.startID).getTokens();
      D(std::println("{} has beed added to the incoming tokens of {} with {} tokens", l, id, t);)
      incomingTokens.push_back(t);
    }
    // NRVO is madated by C++17
    return incomingTokens;
  }

  void saveFinalTokenCount(const std::filesystem::path& path);
#if defined(METRICS) 
  TH(void saveTokenHistory(const std::filesystem::path& path);)
  RAC(void saveReactionActivityCount(const std::filesystem::path& path);)
  RAH(void saveReactionActivityHistory(const std::filesystem::path& path);)
#endif
private: 
  //TODO Make one map that contains transition and place that. 
  //Each elements carreis is typer as an enum
  std::unordered_map<ID, Place> places;
  std::unordered_map<ID, Transition> transitions;
  std::unordered_map<ID, Arc> arcs;
  std::random_device rd;
  std::mt19937 gen {rd()};

#ifdef METRICS
  TH(std::unordered_map<std::string, std::vector<std::size_t>> tokenHistory;)
  RAC(std::unordered_map<std::string, std::size_t> reactionActivity;)
  RAH(std::unordered_map<std::string, std::vector<bool>> reactionActivityHistory;)
#endif
};

}
#endif

