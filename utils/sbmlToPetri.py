#!/usr/bin/env python3
import sys
from pathlib import Path
import cobra
import json
import pandas as pd
import math
from functools import reduce
import warnings
import argparse

def remove_water(model: cobra.Model, h2O_ident:list[str] = ["h2o_e","h2o_c","h2o_p"]) -> cobra.Model:
  # check if list of water metabolites exists as metabolite id
  mod_met_ids = [x.id for x in model.metabolites]
  h2O_ids = []
  for met in h2O_ident:
    if met in mod_met_ids:
      h2O_ids.append(met)
    else:
      warnings.warn(met + " not found as metabolite id in the model, check metabolite ids! Will ignore and proceed")
  h20metabs = list(map(model.metabolites.get_by_id, h2O_ids))
  model.remove_metabolites(h20metabs)
  return(model)

def remove_biomass_func(model: cobra.Model) -> cobra.Model:
  # Check if the reaction exists and remove it
  #print(model.objective)
  #model.reactions.get_by_id(reaction_id).remove_from_model()
  a = [x for x in model.reactions if x.objective_coefficient == 1]
  model.remove_reactions(a)
  print(f"Reaction '{a}' removed.")
  print("------------------------------------")
  return model 


def scale_biomass_func(model: cobra.Model, scaling_factor = 100) -> cobra.Model:
  # Check if the reaction exists and remove it
  #print(model.objective)
  #model.reactions.get_by_id(reaction_id).remove_from_model()
  a = [x for x in model.reactions if x.objective_coefficient == 1][0]
  b = a * (1 / scaling_factor)
  model.remove_reactions([a])
  model.add_reactions([b])
  return model 


def get_non_integer_reactions(model: cobra.Model) -> list[cobra.Reaction]:
  l: list[cobra.Reaction] = []
  for reaction in model.reactions:
    for (metabolite, stoich) in reaction.metabolites.items():
      if int(stoich) != stoich:
        l.append(reaction)
  return l

#if gcd:
#  gcd = reduce(math.gcd,list(map(lambda x: abs(int(x)), new_reaction.metabolites.values())))
#  new_reaction = new_reaction * (1/gcd)
#model.add_reactions([new_reaction])
glob_factor = 1
def scale_reactions(model, global_min_coef):
    global glob_factor
    new_model = cobra.Model()

    # Copy the metabolites, compartments, and other necessary components to the new model
    new_model.compartments = model.compartments.copy()

    exponent = round(math.log10(1 / global_min_coef)) + 1
    factor = 10 ** exponent
    glob_factor = factor
    new_reactions = []  

    for reaction in model.reactions:
        new_reaction = reaction.copy()  
        new_reaction.id = reaction.id  

        scaled_metabolites = {metab: int(round(coeff * factor)) for metab, coeff in reaction.metabolites.items()}
        new_reaction.add_metabolites(scaled_metabolites, combine=False)

        new_reaction.bounds = (int(new_reaction.lower_bound * factor), int(new_reaction.upper_bound * factor))
        new_reactions.append(new_reaction)  
        #print(new_reaction)
        #print(new_reaction.bounds)

    new_model.add_reactions(new_reactions)
    return new_model


def convert_stoichiometry(model: cobra.Model, gcd = False) -> cobra.Model:
  global_min_coef = sys.maxsize
  for reaction in get_non_integer_reactions(model):
      coefficients = reaction.metabolites.values()
      # TODO: Think more about negative coefficients
      smallest_coeff_per_reaction = min(list(map(abs, coefficients)))
      global_min_coef = min(global_min_coef, smallest_coeff_per_reaction)
  

  model = scale_reactions(model, global_min_coef)
  return model

from typing import TypeVar, Optional
K = TypeVar('K')  
V = TypeVar('V') 

def find_by_property(data: dict[K, V], key: K, value: V) -> Optional[tuple[K, V]]:
    return next((item for item in data if item.get(key) == value), None)

def check_metabolite_connections(model, metabolite_id):
    metabolite = model.metabolites.get_by_id(metabolite_id)
    print(f"Metabolite: {metabolite.name}")
    for reaction in metabolite.reactions:
        print(f" - Reaction: {reaction.id} ({reaction.name})")
        if metabolite in reaction.reactants:
            print(f"   Reactant in: {reaction.id}")
        if metabolite in reaction.products:
            print(f"   Product in: {reaction.id}")

def load_model(file: Path):
  
  # Load the SBML file
  model = cobra.io.read_sbml_model(file)
  
  # Display basic information about the model
#  print(f"Model ID: {model.id}")
#  print(f"Number of Reactions: {len(model.reactions)}")
#  print(f"Number of Metabolites: {len(model.metabolites)}")
#  print(f"Number of Genes: {len(model.genes)}")
  
  # Optional: Display a summary of the model
#  model.summary()
  return model

# Writte by ChatGTP
# Adaptep by me
def convert(model: cobra.Model, atp_id:list[str] = ["atp_e","atp_c","atp_p"]):
    # Initialize the data structure
    data = {
        "counter": 0,
        "shapes": []
    }

    # Helper function to create a shape
    def create_shape(shape_id, label, x, y, shape_type, arc_ids, tokens = 10, isABC = False):
        #print("DWDWD", glob_factor)
        shape = {
            "id": shape_id,
            "x": x,
            "y": y,
            "label": label,
            "fillColor": "blue",
            "isSelected": False,
            "arcStart": True,
            "arcEnd": True,
            "arcIDS": arc_ids,
            "tokens": tokens * glob_factor,
            "radius": 10 if shape_type == "Circle" else None,
            "width": 20 if shape_type == "Rectangle" else None,
            "height": 50 if shape_type == "Rectangle" else None,
            "type": shape_type,
            "canFire": False if shape_type == "Rectangle" else None,
            "isABC": isABC
        }
        #print(shape)
        return shape
    def create_arc(arc_id, start_id, end_id, weight = 1, label=""):
        arc = {
                "id": arc_id,
                "label": label,
                "fillColor": "blue",
                "isSelected": False,
                "arcStart": False,
                "arcEnd": False,
                "startID": start_id,
                "endID": end_id, 
                "type": "Arc",
                "edgeWeight": abs(int(weight))
            }
        return arc

    # helper function if reaction is an ABC transporter
    def isABCrxn(rxn:cobra.Reaction, atp_id:list[str] = ["atp_e","atp_c","atp_p"]):
        return(len(rxn.compartments) > 1 and any([x.id in atp_id for x in rxn.metabolites]))
        

    


    # Create shapes for metabolites
    metabolite_ids = {}
    for i, metabolite in enumerate(model.metabolites):
        shape_id = data["counter"]
        metabolite_ids[metabolite.id] = shape_id
        shape = create_shape(shape_id, metabolite.id, 100 * (i % 10), 100 * (i // 10), "Circle", [], 10)
        data["shapes"].append(shape)
        data["counter"] += 1
    MAT = cobra.util.create_stoichiometric_matrix(model, array_type = "DataFrame")

    # Create shapes for reactions and arcs
    for reaction in model.reactions:
        # Create reverse reaction
        if not reaction.id.startswith("EX_") and (reaction.reversibility == True or (reaction.lower_bound < 0 and reaction.upper_bound > 0)): 
          shape_id = data["counter"]
          reaction_id = shape_id
          shape = create_shape(shape_id, "REV_" + reaction.id, 100 * (shape_id % 10), 100 * (shape_id // 10) + 50, "Rectangle", [], isABC= isABCrxn(reaction, atp_id = atp_id))
          data["shapes"].append(shape)
          data["counter"] += 1


          # Create arcs for reactants
          # TODO I think we can merge this code into one loop
          for reactant in reaction.reactants:
              arc_id = data["counter"]
              # Create an arc for each reactant_place to the reaction
              arc = None
              #print("FROM", reaction.id , "TO", reactant.id)
              weight = MAT.loc[reactant.id, reaction.id]
              #print(weight)
              arc = create_arc(arc_id, reaction_id, metabolite_ids[reactant.id], weight,
                               label =reaction.id + "(" + str(reaction_id) + ")|" +
                               reactant.id+ "(" + str(metabolite_ids[reactant.id]) +")")
              data["shapes"].append(arc)
              data["counter"] += 1
              data["shapes"][metabolite_ids[reactant.id]]["arcIDS"].append(arc_id)
              data["shapes"][reaction_id]["arcIDS"].append(arc_id)
              #print(reaction_id)

          # Create arcs for products
          for product in reaction.products:
              arc_id = data["counter"]
              # Create an arc for reaction to each product place
              #print("FROM", reaction.id , "TO", product.id)
              weight = MAT.loc[product.id, reaction.id]
              #print(weight)
              arc =  create_arc(arc_id,  metabolite_ids[product.id], reaction_id, weight, 
                               label =reaction.id + "(" + str(reaction_id) + ")|" +
                               product.id+ "(" + str(metabolite_ids[product.id]) +")")
              data["shapes"].append(arc)
              data["counter"] += 1
              data["shapes"][reaction_id]["arcIDS"].append(arc_id)
              data["shapes"][metabolite_ids[product.id]]["arcIDS"].append(arc_id)
        
        shape_id = data["counter"]
        reaction_id = shape_id
        shape = create_shape(shape_id, reaction.id, 100 * (shape_id % 10), 100 * (shape_id // 10) + 50, "Rectangle", [], isABC= isABCrxn(reaction, atp_id = atp_id))
        data["shapes"].append(shape)
        data["counter"] += 1



        # Create arcs for reactants
        # TODO I think we can merge this code into one loop
        for reactant in reaction.reactants:
            arc_id = data["counter"]
            # Create an arc for each reactant_place to the reaction
            arc = None
            #print("FROM", reaction.id , "TO", reactant.id)
            weight = MAT.loc[reactant.id, reaction.id]
            #print(weight)
            if reaction.id.startswith("EX_") and reaction.upper_bound == 0 and reaction.lower_bound < 0:
              # This reactions takes up stuff (aka generates out of nothing) (source)
              arc = create_arc(arc_id,  reaction_id, metabolite_ids[reactant.id], weight,
                               label =reaction.id + "(" + str(reaction_id)+ ")|" +
                               reactant.id+ "(" + str(metabolite_ids[reactant.id])+")")
              met = find_by_property(data["shapes"], "id", metabolite_ids[reactant.id])
              met["tokens"] = -1 * reaction.lower_bound
              #print("REACTION IS UPTAKE")
              data["shapes"].append(arc)
              data["counter"] += 1
              data["shapes"][metabolite_ids[reactant.id]]["arcIDS"].append(arc_id)
              data["shapes"][reaction_id]["arcIDS"].append(arc_id)
            elif reaction.id.startswith("EX_") and reaction.upper_bound > 0 and reaction.lower_bound == 0:
              # This reactions pumps stuff (aka pumps into nothing) (sink)
              #print(f'REACTION {reaction.id} IS SINK')
              arc = create_arc(arc_id, metabolite_ids[reactant.id], reaction_id, weight,
                               label =reaction.id + "(" + str(reaction_id) + ")|" +
                               reactant.id+ "(" + str(metabolite_ids[reactant.id])+")")
              met = find_by_property(data["shapes"], "id", metabolite_ids[reactant.id])
              met["tokens"] = 0
              data["shapes"].append(arc)
              data["counter"] += 1
              data["shapes"][metabolite_ids[reactant.id]]["arcIDS"].append(arc_id)
              data["shapes"][reaction_id]["arcIDS"].append(arc_id)
            elif reaction.id.startswith("EX_"):
              # ELSE we have a reversible ex reaction                           
              arc = create_arc(arc_id, metabolite_ids[reactant.id], reaction_id, weight,
                               label =reaction.id + "(" + str(reaction_id) + ")|" +
                               reactant.id+ "(" + str(metabolite_ids[reactant.id]) +")")
              #print(f"REVERSIBLE  {reaction.id} EX REACTION")
              met = find_by_property(data["shapes"], "id", metabolite_ids[reactant.id])
              met["tokens"] = -1 * reaction.lower_bound
              data["shapes"].append(arc)
              data["counter"] += 1
              data["shapes"][metabolite_ids[reactant.id]]["arcIDS"].append(arc_id)
              data["shapes"][reaction_id]["arcIDS"].append(arc_id)


              shape_id = data["counter"]
              reaction_id = shape_id
              shape = create_shape(shape_id, "REV_" + reaction.id, 100 * (shape_id % 10), 100 * (shape_id // 10) + 50, "Rectangle", [], isABC= isABCrxn(reaction, atp_id = atp_id))
              data["shapes"].append(shape)
              data["counter"] += 1
              
              arc_id = data["counter"]
              arcR = create_arc(arc_id,  reaction_id, metabolite_ids[reactant.id], weight,
                                label =reaction.id + "(" + str(reaction_id) + ")|" +
                                reactant.id+ "(" + str(metabolite_ids[reactant.id]) +")")
              data["shapes"].append(arcR)
              data["counter"] += 1
              data["shapes"][metabolite_ids[reactant.id]]["arcIDS"].append(arc_id)
              data["shapes"][reaction_id]["arcIDS"].append(arc_id)

            else:
              arc = create_arc(arc_id, metabolite_ids[reactant.id], reaction_id, weight,
                               label =reaction.id + "(" + str(reaction_id) + ")|" +
                               reactant.id+ "(" + str(metabolite_ids[reactant.id]) +")")
              data["shapes"].append(arc)
              data["counter"] += 1
              data["shapes"][metabolite_ids[reactant.id]]["arcIDS"].append(arc_id)
              data["shapes"][reaction_id]["arcIDS"].append(arc_id)
              #print(reaction_id)

        # Create arcs for products
        for product in reaction.products:
            arc_id = data["counter"]
            # Create an arc for reaction to each product place
            #print("FROM", reaction.id , "TO", product.id)
            weight = MAT.loc[product.id, reaction.id]
            #print(weight)
            arc =  create_arc(arc_id, reaction_id, metabolite_ids[product.id], weight,
                              label =reaction.id + "(" + str(reaction_id) + ")|" +
                                     product.id+ "(" + str(metabolite_ids[product.id]) +")")
            data["shapes"].append(arc)
            data["counter"] += 1
            data["shapes"][reaction_id]["arcIDS"].append(arc_id)
            data["shapes"][metabolite_ids[product.id]]["arcIDS"].append(arc_id)

    return data


# Examoke get_json_id_of_metab(g6p_c, ..)
def get_arcs_of_metab(json, metabolite_name: str) -> Optional[list[int]]:
  for elem in json["shapes"]:
    if elem["label"] == metabolite_name:
      return elem["arcIDS"]
  return None

def set_metabs_constant(json, ids: list[str]):
  if len(ids) == 0:
    return json
  arc_ids: list[Optional[list[int]]] = []

  for id in ids:
    arc_ids.append(get_arcs_of_metab(json, id))
  
  arc_ids = list(filter(None, arc_ids))

  # Flatten
  arc_ids = list(set(reduce(lambda x, y: x + y, arc_ids)))
  
  for elem in json["shapes"]:
    if elem["id"] in arc_ids:
      elem["edgeWeight"] = 0
  return json

def findAllExternalMetabolites(mod: cobra.Model):
  # simple function to detect and return all external metabolites of the model
  # returns a list of metabolites IDs
  # can be used to fix all external metabolites concentrations when generating the json model
  return([x.id for x in mod.metabolites if x.compartment == "e"])

def adjustMedium(model:cobra.Model, medium:Path):
  # load the medium file and adjust the models medium

  df = pd.read_csv(medium)
  medium_dict = dict(zip(df.iloc[:, 0], df.iloc[:, 1]))
  model.medium = medium_dict
  return(model)


if __name__ == '__main__':
  
  # define the argparser and which arguments are accepted/expected
  parser = argparse.ArgumentParser(description = "Transform an SBML model to JSON format to make it compatible with PetriNetSimulator")
  parser.add_argument("--infile", required = True, help = "Path to the input model in SBML format")
  parser.add_argument("--outfile", required = True, help = "Path to the output model in JSON format")
  parser.add_argument("--const_metabolites", default = "None", help = "Path to a json holding the informaton which metabolites should be kept constant (should not be consumed/produced during the simulation). Can be set to 'all' to fix all external metabolites or 'none' (default) to ")
  parser.add_argument("--remove_biomass", action = "store_true", default = False, help = "Should the biomass function be removed?")
  parser.add_argument("--biomass_scaling_factor", default = 100, help = "Biomass scaling factor, stoichometry of the biomass will be devided by this factor")
  parser.add_argument("--h2o_id", default = ["h2o_e","h2o_c","h2o_p"], nargs = "+", help = "Define water metabolite in the model")
  parser.add_argument("--atp_id", default = ["atp_e","atp_c","atp_p"], nargs = "+", help = "Define the atp metabolites in the model, needed to define ABC reactions")
  parser.add_argument("--medium", default = "None", help = "A csv containing the medium composition which should be assumed")

  args = parser.parse_args()
  print(args)
  
  # get the input model
  input_file:Path = Path(args.infile)
  model: cobra.Model = load_model(input_file)
  
  # adjust the diet if available
  if args.medium != "None":
    model = adjustMedium(model, args.medium)

  # define  metabolites which should not be consumed/produced by the model and whose concentration should be constant
  if args.remove_biomass == True:
    model = remove_biomass_func(model)
  else:
    model = scale_biomass_func(model, args.biomass_scaling_factor)

  # There are also other reactions with non-integer stoichiometry
  model = convert_stoichiometry(model, gcd = False)
  assert(get_non_integer_reactions(model) == [])

  # remove water
  h2os:list[str] = args.h2o_id
  if len([x.id for x in model.metabolites if x.id in h2os]) == 0:
    warnings.warn("No h2o found in the model, set '--h2o_id h2o_e h2o_p ...' to the correct values in your model") 
  model = remove_water(model, h2O_ident = args.h2o_id)
  
  # create the actual json
  atps:list[str] = args.atp_id
  if len([x.id for x in model.metabolites if x.id in atps]) == 0:
    warnings.warn("No ATP found in the model, set '--atp_id atp_e atp_p ...' to the correct values in your model") 
  custom_json = convert(model)

  # define those metabolites which should be held constant
  if args.const_metabolites.lower() == "all":
    const_mets = findAllExternalMetabolites(model)
  elif args.const_metabolites.lower() == "none":
    const_mets = []
  else: 
    with open(args.const_metabolites, "r") as ff:
      const_mets = json.load(ff)

  print("Constant metabolites")
  print(const_mets)
  custom_json = set_metabs_constant(custom_json, const_mets)
  # add the scaling factor to the json
  custom_json["factor"] = glob_factor

  #cobra.io.write_sbml_model(model, "transformed.xml")
  # write the model
  output_file: Path = Path(args.outfile)
  with open(output_file, "w") as outfile:
      json.dump(custom_json, outfile, indent=2)



