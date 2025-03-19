# Building
1. Clone the repository
2. Create conda environment from given .yml
    `mamba env create -f environment.yml`
3. Activate simulator environment
    `mamba activate simulator`
4. Build the application
    `sh compile.sh`    
5. Run the application  
    `./Build/bin/simulator data/examples/glycolysis_selfmade.json `

# Build with Clion
See (isssue14)[https://github.com/Unaimend/PetriNet/issues/14]
# Running
The application can be configured via the *config.json*. 

**threads**: Determines the amount of instances of the program that will run.
Will create the appropriate output files for each instance.

**mode**: *iteration* or *stopping_criterion*. If iteration is choosren the simulation we run for the given number of iterations. If stopping_criterion is choosen the simulation will stop once the given threshold is reached OR the `std. deviation` is below the threshold given in `stdDevThresh`.

**iterations**: Determines for how many iterations the simulation will run in both modes. Note that in **stopping_criterion** mode the simulation might end earlier.

**history_length**: Determines the length of the history that will be taken into account for the std. deviation.

**stdDevThresh**: Determines at which low the std. dev is allowed to be for the last `history_length` iterations. Note: If at least one metaboltie shows a bigger std. deviation the simulation will continue to run.

# Analyzing
1. Execute the plot script 
    `python3 scripts/plot.py`
2. Execute the analysis script
    `Rscript analysis.R`



