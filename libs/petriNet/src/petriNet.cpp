#include <iostream> 
#include "../include/json.hpp"
#include "../include/petriNet.hpp"
#include <iostream>
#include <fstream>
#include <print>

void helloFromLib() {
  std::cout << "Hello from lib!\n";
}

namespace petrinet {
  using json = nlohmann::json;


  void PetriNet::loadFromJSON(const std::filesystem::path& path) {
    std::ifstream input_file{path};
    if (!input_file.is_open()) {
      std::cerr << "Could not open the file!" << std::endl;
    }

    json parsed_json;
    input_file >> parsed_json;

    auto shapes = parsed_json["shapes"];
    for (const auto& s: shapes) {
      if(s["type"] == "Rectangle") {
        ID id = s["id"];
        std::string label = s["label"];
        std::vector<ID> arcIDs = s["arcIDS"];
        std::println("Transition {} initialized with id {}", label, id);
        for (ID i : arcIDs) {
          std::println("Arc {}", i);
        }
        addTransition(id, Transition{id, std::move(label), std::move(arcIDs)});
      } else if(s["type"] == "Circle") {
        ID id = s["id"];
        std::string label = s["label"];
        std::vector<ID> arcIDs = s["arcIDS"];
        std::size_t tokens = s["tokens"];
        std::println("Place {} initialized with id {} and {} tokens", label, id, tokens);
        for (ID i : arcIDs) {
          std::println("Arc {}", i);
        }
        addPlace(id, Place{id, std::move(label), std::move(arcIDs), tokens});
      }  else if (s["type"] == "Arc") {
        ID id = s["id"];
        ID startID = s["startID"];
        ID endID = s["endID"];
        ID edgeWeight = s["edgeWeight"];
        std::println("Arc initialized with id {} from {} to {} width weight {}", id, startID, endID, edgeWeight);

        addArc(id, {id, startID, endID, edgeWeight});
      } else {
        std::cerr << "UNK" << std::endl;
      }
    }
    input_file.close();
  }
}
