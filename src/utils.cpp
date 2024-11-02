#include "utils.hpp"
#include "../libs/petriNet/include/json.hpp"
#include "fstream"
#include "iostream"
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


double mean(const std::vector<double>& x) { 
  return std::accumulate(x.begin(),x.end(), 0.0)/static_cast<double>(x.size());
}


double varianceOpt(const std::vector<double>& x, double mean) {
  // TODO I think the std math functions are slow as
  const auto squaredDiffs = x | std::views::transform([mean](double x) {
      double diff = x - mean;
      return diff * diff;
  });
  return std::accumulate(squaredDiffs.begin(), squaredDiffs.end(), 0.0)/static_cast<double>(x.size());
}

double stdDevOpt(const std::vector<double>& x, double mean) {
  // TODO I think the std math functions are slow as
  auto var = varianceOpt(x, mean);
  return std::sqrt(var);
}


