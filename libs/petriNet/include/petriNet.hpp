// TODO We could remap all the ids to make them contiguous and then use std::vectors for accessing the things
// When doing this we have to make sure that that we have the different types ordered, because based on an ID
// we ne do decide(FAST!!!) if an od belongs to and arc or a vec
// When doing this it might be good to keep the clases AS SMALL as possible SSO for strings (label member) might be problematic there
#ifndef PETRINET_HPP
#define PETRINET_HPP
#include <cassert>
#include <cstddef>
#include <unordered_map>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "../../../src/utils.hpp"
#define METRICS
#define TOKEN_HISTORY
#define REACTION_ACTIVITY_HISTORY
#define REACTION_ACTIVITY_COUNT
#define BLOCKED_BY_COUNT
#define RUNNING_AGAINST_GRADIENT

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

#ifdef BLOCKED_BY_COUNT
#define BBC(...) __VA_ARGS__
#else
#define BBC(x) 
#endif

#ifdef RUNNING_AGAINST_GRADIENT
#define RAG(...) __VA_ARGS__
#else
#define RAG(...)
#endif

// TODO CHECK SIZER AND ORDER OF MEMBERS
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
    id{id}, label{std::move(label)}, arcs{std::move(arcs)}, tokens{tokens}   {
  }

#ifdef METRICS
  //maybe replace by 
  //copy(const Place& p)
  //{
  //  return Place(p.get..)
  //}
  explicit Place(const Place& that) : id{that.id}, label{that.label}, arcs{that.arcs}, tokens{that.tokens} {
    // TODO CHeck that this is is not used in high-performance mode aka when metrics are turned off
  }
#endif

  Place& operator=(const Place&) = delete;
//  TODO Check what the default move constructor does
  Place(Place&&) noexcept = default ;
  Place& operator=(Place&&) noexcept = default;
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
  Transition(ID id, std::string&& label, std::vector<ID>&& arcs) noexcept : id{id}, label{std::move(label)}, arcs{std::move(arcs)}  {
  }
  Transition(const Transition&) = delete;
  Transition& operator=(const Transition &) = delete;
//  TODO Check what the default move constructor does
  Transition(Transition&&) noexcept = default;
  Transition& operator=(Transition&&) noexcept = default;
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

  const std::unordered_map<ID, Transition>& getTransitions() const & {  
    return transitions;
  }


  const std::unordered_map<ID, Place>& getPlaces() const & {  
    return places;
  }


  const std::unordered_map<ID, Arc>& getArcs() const & {  
    return arcs;
  }

  void toDot(const std::filesystem::path& path);

  void saveHistory() {
#if defined(METRICS) 
#ifdef TOKEN_HISTORY
      for(auto& [id, place] : places) {
        D(std::println("Place {} contains tokens {} at iteration {}", place.getID(), place.getTokens(), N));
        tokenHistory[place.getLabel()].push_back(place.getTokens());
      }
#endif
#endif
  }


  inline void simulateNShuffe(int N) {
    while((N--) != 0) {
      std::cout << N << std::endl;
      saveHistory();
      simulateSingleGradient();
    }
    saveHistory();
  }

  void simulateStoppingCritertion (const uint64_t historyLength = 5, const uint64_t hardThresh = 1'000, const double stdDevThresh = 0.5 ) {
    uint64_t counter = 0;
    uint64_t iterationCounter = 0;
    // How often we check if we should stop
    const uint64_t saveTimepoint = 5;
    assert(saveTimepoint >= historyLength);
    bool continueRunning = true;
    //TODO This should brake aour analysis scripts because runs might be a different length 
    while(continueRunning) {
      if (iterationCounter == hardThresh) { 
        break;
      }
      ++iterationCounter;
      simulateSingleGradient();
      saveHistory();
      counter++;
      if(counter == saveTimepoint) {
        std::vector<double> cvs;
        for(const auto& [label, tokens]: tokenHistory) {
          //TODO Fix or check somehow
          auto start = tokens.end() - historyLength;
          auto end = tokens.end();
          std::vector<double> last_elements(start, end);
          auto m = mean(last_elements);
          auto cv = stdDevOpt(last_elements, m);
          cvs.push_back(cv);
        }
        bool change = false;
        // Iterate over the stddevs for the last five runs of all the metabolites.
        // If the std dev is greater than 2, we assume there is still change in the metabolism
        // and stop checking and go to the next tiemstep. If there is not metabolite with a 
        // stddev > 2 we assume everything is blocked and stop the simulation
        for(auto cv : cvs) {
          //TODO This stupid, metabolites might be on totally different scales.
          if(cv > stdDevThresh) {
            change = true;
            break;
          }              
        }
        continueRunning = change;
        counter = 0;
      }
    }
  }


  inline void simulateGivenRxns(const std::vector<ID>& keys) {
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
      bool reactionAllowed = true;

      for(const Arc& arc: inComingArcs) {
        auto t = places.at(arc.startID).getTokens();
        if (t < arc.edgeWeight) {
          reactionAllowed = false;
        }
      }
#ifdef METRICS

#ifdef BLOCKED_BY_COUNT
      // Because we only fire if the sum of all input tokens is bigger then sum of all output tokens
      // it would be interesting to know who is responsible for blocking the reactiong
      // id is a transition id
      for(const Arc& arc: inComingArcs) {
        // We might be able to use the data from getIncomingTokens from above
        // inComingArcs are for the current reaction.
        // Now we go over all incoming arcs and get their labels, tokens, and edge weights
        // and check if the reaction can't fire because of them
        const std::string& l = places.at(arc.startID).getLabel();
        auto t = places.at(arc.startID).getTokens();
        //I the token of the place at arc.startId are smaller than it's weight
        // it can't fire
        if (t < arc.edgeWeight) {
          //l is a blocking input metabolite
          // Current transition
          const auto& rl = transitions.at(id).getLabel();
          blockedByCount.insert({rl, {}});
          blockedByCount.at(rl)[l] += 1;
        }
      }
#endif
#endif
      auto outSum = std::accumulate(outgoingTokens.begin(), outgoingTokens.end(), static_cast<std::size_t>(0));
      auto incSum = std::accumulate(incomingTokens.begin(), incomingTokens.end(), static_cast<std::size_t>(0));

#ifdef METRICS
#ifdef RUNNING_AGAINST_GRADIENT
      // The gradient is to big for the ennzyme to fire
      if (outSum >= incSum) {
        const auto& rl = transitions.at(id).getLabel();
        runningAgainstGradient.insert({rl, {}});
        unsigned long sumIn = 0;
        for(const Arc& arc: inComingArcs) {
          // Go through all orcs and get their label
          const std::string& l = places.at(arc.startID).getLabel();
          auto t = places.at(arc.startID).getTokens();
          sumIn += t;
          runningAgainstGradient.at(rl)["in: " + l] = t;
        }
        runningAgainstGradient.at(rl)["inSum"] = sumIn;
        unsigned long sumOut = 0;
        for(const Arc& arc: outGoingArcs) {
          // Go through all orcs and get their label
          const std::string& l = places.at(arc.endID).getLabel();
          auto t = places.at(arc.endID).getTokens();
          sumOut += t;
          runningAgainstGradient.at(rl)["out: " + l] = t;
        }
        runningAgainstGradient.at(rl)["outSum"] = outSum;
      }
#endif
#endif
      if(!incomingTokens.empty() && !outgoingTokens.empty() && reactionAllowed == true && incSum > outSum) {
        //D(std::cout <<  << *minIncToken << " " << incSum << " " << outSum << "\n";)
        M(const auto& label = transitions.at(id).getLabel();)
        RAC(reactionActivity[label]++;)
        RAH(reactionActivityHistory[label].push_back(true);)
        D(std::println("{} has fired", id);)
        for(const Arc& arc: outGoingArcs) {
          auto t = places.at(arc.endID).getTokens();
          places.at(arc.endID).setTokens(t + arc.edgeWeight);
        }

        for(const Arc& arc: inComingArcs) {
          auto t = places.at(arc.startID).getTokens();
          assert(t >= arc.edgeWeight);
          places.at(arc.startID).setTokens(t - arc.edgeWeight);
        }
      } // This enables the infinit generation of tokens from Exchanve reactions
      /* else if(inComingArcs.empty() && !outGoingArcs.empty()) {
        M(const auto& label = transitions.at(id).getLabel();)
        RAC(reactionActivity[label]++;)
        RAH(reactionActivityHistory[label].push_back(true);)
        D(std::println("{} has fired", id);)
        for(const Arc& arc: outGoingArcs) {
          auto t = places.at(arc.endID).getTokens();
          places.at(arc.endID).setTokens(t+1);
        }
      } */ 
      else {
        RAH(reactionActivityHistory[transitions.at(id).getLabel()].push_back(false);)
      }
    }
  }

  inline void simulateSingleGradient() {
    // Vector to store the keys
    // TODO Preallocate the right capacity
    std::vector<ID> keys;
  
    // Iterate over the map and store keys
    for (const auto& pair : transitions) {
        keys.push_back(pair.first);
    }
    
    std::ranges::shuffle(keys, gen);

    simulateGivenRxns(keys);
    
  }

  void setTokens(ID id, std::size_t tokens) {
    places.at(id).setTokens(tokens);
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
      // Because we are iterating over transitions and looking at arcs that 
      // leave me I know that arc.endID refers to a place
      auto l = places.at(arc.endID).getLabel();
      auto t = places.at(arc.endID).getTokens();
      D(std::println("{} has beed added to the outgoing tokens of {} with {} tokens", l, id, t);)
      outgoingTokens.push_back(t);
    }
    // NRVO is mandated by C++17
    return outgoingTokens;
  }


  std::vector<std::size_t> getIncomingTokens(const std::vector<Arc>& inComingArcs) {
    auto incomingTokens = std::vector<std::size_t>{};
    for(const Arc& arc: inComingArcs) {
      D(std::println("Arc {} coming into {}",arc.id, id);)
      // Because we are iterating over transitions and looking at arcs that 
      // coming in to me I know that arc.startID refers to a place
      D(const auto& l = places.at(arc.startID).getLabel(););
      const auto& t = places.at(arc.startID).getTokens();
      D(std::println("{} has beed added to the incoming tokens of {} with {} tokens", l, id, t);)
      incomingTokens.push_back(t);
    }
    // NRVO is mandated by C++17
    return incomingTokens;
  }

  void saveFinalTokenCount(const std::filesystem::path& path);
#if defined(METRICS) 
  TH(void saveTokenHistory(const std::filesystem::path& path);)
  RAC(void saveReactionActivityCount(const std::filesystem::path& path);)
  RAH(void saveReactionActivityHistory(const std::filesystem::path& path);)
  BBC(void saveBlockedByCount(const std::filesystem::path& path);void saveRunningAgainstGradient(const std::filesystem::path &path);)

#endif
private: 
  //TODO Make one map that contains transition and place that. 
  //Each elements carries is type as an enum
  std::unordered_map<ID, Place> places;
  std::unordered_map<ID, Transition> transitions;
  std::unordered_map<ID, Arc> arcs;
  std::random_device rd;
  std::mt19937 gen {rd()};

#ifdef METRICS
  TH(std::unordered_map<std::string, std::vector<std::size_t>> tokenHistory;)
  RAC(std::unordered_map<std::string, std::size_t> reactionActivity;)
  RAH(std::unordered_map<std::string, std::vector<bool>> reactionActivityHistory;)
  BBC(std::unordered_map<std::string, std::unordered_map<std::string, std::size_t>> blockedByCount;)
  RAG(std::unordered_map<std::string, std::unordered_map<std::string, std::size_t>> runningAgainstGradient;)
#endif
};

}
#endif

