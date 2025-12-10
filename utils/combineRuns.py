# Porthmeus
# 21.10.25

# Combining the output files of several consecutive runs of the same simulation into one common file.

import json
import argparse
import os

def readDirs(indirs:str):
    ''' Put the input files in sorted dictionaries '''
    
    files = {"FTC" : [],
             "RAC" : [],
             "RAH" : [],
             "BBC" : [],
             "TH" : [],
             "RAG" : []}
    for path in indirs:
        if os.path.exists(path) and os.path.isdir(path):
            fs = os.listdir(path)
            files["FTC"].append(os.path.join(path, sorted([x for x in fs if x.startswith("FTC") and x.endswith("json")])[0]))
            files["RAC"].append(os.path.join(path,sorted([x for x in fs if x.startswith("RAC") and x.endswith("json")])[0]))
            files["RAH"].append(os.path.join(path,sorted([x for x in fs if x.startswith("RAH") and x.endswith("json")])[0]))
            files["BBC"].append(os.path.join(path,sorted([x for x in fs if x.startswith("BBC") and x.endswith("json")])[0]))
            files["RAG"].append(os.path.join(path,sorted([x for x in fs if x.startswith("RAG") and x.endswith("json")])[0]))
            files["TH"].append(os.path.join(path, sorted([x for x in fs if x.startswith("TH") and x.endswith("json")])[0]))
        else:
            raise(IOError("Directory {path} does not exist".format(path = path)))



    

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description = "This script will take and combine the output files of several consecutive runs of the same simulation into one common file. Input is the directories of the simulations and files will be simply expanded in the order of the input directories given. Further, if several runs are present (several threads run), only the first will be used, so make sure you know what you are doing.")
    parser.add_argument("-i","--input_dirs", required = True, nargs = "+", help = "Path to the input directories")
    parser.add_argument("-o","--output_dir", default = ".", help = "Path to the output directory, default is the current working directory")

    args = parser.parse_args()


