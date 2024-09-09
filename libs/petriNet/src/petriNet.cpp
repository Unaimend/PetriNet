#include <iostream> 
#include <fstream>
#include <print>
#include <vector>
#include <utility> //std::move
#include <cstddef>
#include <string>

#include "../include/json.hpp"
#include "../include/petriNet.hpp"

void helloFromLib() {
  std::cout << "Hello from lib!\n";
}

namespace petrinet {
  using json = nlohmann::json;


  //TODO Replace bei nlohman code
  void PetriNet::loadFromJSON(const std::filesystem::path& path) {
    std::ifstream input_file{path};
    if (!input_file.is_open()) {
      std::cerr << "Could not open the file!" << std::endl;
      throw std::runtime_error("File not found: " / path);
    }

    json parsed_json;
    input_file >> parsed_json;

    auto shapes = parsed_json["shapes"];
    for (const auto& s: shapes) {
      if(s["type"] == "Rectangle") {
        ID id = s["id"];
        std::string label = s["label"];
        std::vector<ID> arcIDs = s["arcIDS"];
        D(std::println("Transition {} initialized with id {}", label, id);)
        D(for (ID i : arcIDs) {
          std::println("Arc {}", i);
        })
        addTransition(id, Transition{id, std::move(label), std::move(arcIDs)});
      } else if(s["type"] == "Circle") {
        ID id = s["id"];
        std::string label = s["label"];
        std::vector<ID> arcIDs = s["arcIDS"];
        std::size_t tokens = s["tokens"];
        D(std::println("Place {} initialized with id {} and {} tokens", label, id, tokens);)
        D(for (ID i : arcIDs) {
          std::println("Arc {}", i);
        })
        addPlace(id, Place{id, std::move(label), std::move(arcIDs), tokens});
      }  else if (s["type"] == "Arc") {
        ID id = s["id"];
        ID startID = s["startID"];
        ID endID = s["endID"];
        ID edgeWeight = s["edgeWeight"];
        D(std::println("Arc initialized with id {} from {} to {} with weight {}", id, startID, endID, edgeWeight);)

        addArc(id, {id, startID, endID, edgeWeight});
      } else {
        std::cerr << "UNK" << std::endl;
      }
    }
    input_file.close();
  }

  void PetriNet::toDot(const std::filesystem::path& path) {
    std::ofstream output_file{path};
    if (!output_file.is_open()) {
      std::cerr << "Could not open the file!" << std::endl;
    }
    output_file << "digraph PetriNet {" << std::endl;

    // Define place nodes
    for (const auto& [id, place] : places) {
      output_file << "  " << id << " [shape=circle, label=\"" << place.getLabel() << "\n" << place.getTokens() << "\"];" << std::endl; 
    }

    // Define transition nodes
    for (const auto& [id, transition] : transitions) {
      output_file << "  " << id << " [shape=box, label=\"" << transition.getLabel() << "\"];" << std::endl;
    }
    
    for (const auto& [id, arc] : arcs) {
      output_file << "  " << arc.startID << " -> " << arc.endID << ";" << std::endl;
    }

    output_file << "}" << std::endl;
    output_file.close();
  }

  void PetriNet::saveFinalTokenCount(const std::filesystem::path& path) {
    json j;
    for(const auto& [id, place] : places) {
      j[place.getLabel()] = place.getTokens();
    }
    std::ofstream o(path);

    if(o.is_open()) {
      o << j.dump(2);
    }
    o.close();
  }
#ifdef METRICS
#ifdef TOKEN_HISTORY
  void PetriNet::saveTokenHistory(const std::filesystem::path& path) {
    json j;
    j = tokenHistory;
    std::ofstream o(path);

    if(o.is_open()) {
      o << j.dump(2);
    }
    o.close();

  }
#endif
#ifdef REACTION_ACTIVITY_COUNT
  void PetriNet::saveReactionActivityCount(const std::filesystem::path& path) {
    json j;
    j = reactionActivity;
    std::ofstream o(path);

    if(o.is_open()) {
      o << j.dump(2);
    }
    o.close();

  }
#endif 
#ifdef REACTION_ACTIVITY_HISTORY
  void PetriNet::saveReactionActivityHistory(const std::filesystem::path& path) {
    json j;
    j = reactionActivityHistory;
    std::ofstream o(path);

    if(o.is_open()) {
      o << j.dump(2);
    }
    o.close();

  }
#endif
#endif
}
