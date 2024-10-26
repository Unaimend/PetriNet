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

# Running
The application can be configured via the *config.json*. 

**threads**: Determines the amount of instances of the program that will run.
Will create the appropriate output files for each instance.

**iterations**: Determines for how many iterations the simulation will run

# Analyzing
1. Execute the plot script 
    `python3 scripts/plot.py`
2. Execute the analysis script
    `Rscript analysis.R`



