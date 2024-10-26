#include "utils.hpp"

// Convert JSON to MyClass
static void from_json(const nlohmann::json &j, Config& myClass) {
    j.at("threads").get_to(myClass.threads);
    j.at("iterations").get_to(myClass.iterations);
}

Config loadConfig(const std::filesystem::path& p) {
  std::ifstream input_file{p};
  if (!input_file.is_open()) {
    std::cerr << "Could not find config file" << std::endl;
    exit(1);
  }
  
  nlohmann::json parsed_json;
  input_file >> parsed_json;
  Config c = parsed_json.get<Config>();
  return c;

}
