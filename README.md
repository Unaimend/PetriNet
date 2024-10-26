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

# Analyzing
1. Execute the plot script 
    `python3 scripts/plot.py`
2. Execute the analysis script
    `Rscript analysis.R`



