#!/usr/bin/env python3
# Porthmeus
# 17.10.25

# This script will take a petri net model JSON file and the FTC_XX.json file(s) from a previous simulation and adjust the tokens in the model to the ones in the FTC file. This prepares the model for continuing the previous run.

from pathlib import Path
import json
import argparse
import os

def copyValues(model:Path, ftc:Path) -> dict:
    # read the files
    with open(ftc, "r") as ff:
        ftc = json.load(ff)
    with open(model, "r") as ff:
        model = json.load(ff)

    # adjust the model
    mets = ftc.keys()
    shapes = model["shapes"]
    for shape in model["shapes"]:
        if shape["label"] in mets:
            shape["tokens"] = ftc[shape["label"]]
    return(model)

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description = "This script will take a petri net model JSON file and the FTC_XX.json file(s) from a previous simulation and adjust the tokens in the model to the ones in the FTC file. This prepares the model for continuing the previous run.")
    parser.add_argument("-m","--model", required = True, help = "Path to the input model in JSON format")
    parser.add_argument("-t", "--tokens", required = True, nargs = "+", help = "Path to the input FTC table(s)")

    args = parser.parse_args()
    
    # create the models
    models = [copyValues(model = Path(args.model), ftc = Path(x)) for x in args.tokens]
    ftc_names = [os.path.split(os.path.splitext(x)[0])[-1] for x in args.tokens]
    model_name = os.path.split(os.path.splitext(args.model)[0])[-1]
    new_names = [model_name + "_" + x +".json" for x in ftc_names]
    for i in range(len(models)):
        with open(new_names[i], "w") as outfile:
            json.dump(models[i], outfile, indent = 2)

