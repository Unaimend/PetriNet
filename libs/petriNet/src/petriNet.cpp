#include <iostream> 
#include "../include/json.hpp"
#include "../include/petriNet.hpp"
#include <iostream>
#include <fstream>

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

    input_file.close();

    std::cout << parsed_json.dump(4) << std::endl; 
  }
}
