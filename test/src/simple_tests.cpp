#include "../include/boost/ut.hpp" // import boost.ut;
#include "../../libs/petriNet/include/petriNet.hpp" // import boost.ut;


constexpr auto sum(auto... values) { return (values + ...); }

int main() {
  using namespace boost::ut;


  "path does not exist"_test = [] {
    petrinet::PetriNet p;
    expect(throws([&] { p.loadFromJSON("non_existing.json"); })) << "throws any exception";
  };


  "malformed json"_test = [] {
    petrinet::PetriNet p;
    expect(throws([&] { p.loadFromJSON("../../test/examples/loading/malformed_json.json"); })) << "throws nlohman::json exception";
  };

  "circle loading sucess"_test = [] {
    petrinet::PetriNet p;
    p.loadFromJSON("../../test/examples/loading/circle.json");
    expect(p.getPlaces().size() == 1);
    expect(p.getTransitions().size() == 0);
    expect(p.getArcs().size() == 0);

    expect(p.getPlaces().at(0).getTokens() == 0);
    expect(p.getPlaces().at(0).getLabel() == "");
  };


  "rectangle loading sucess"_test = [] {
    petrinet::PetriNet p;
    p.loadFromJSON("../../test/examples/loading/rectangle.json");
    expect(p.getPlaces().size() == 0);
    expect(p.getTransitions().size() == 1);
    expect(p.getArcs().size() == 0);

    expect(p.getTransitions().at(1).getLabel() == "");
    expect(p.getTransitions().at(1).getID() == 1);
  };
}
