import json
import matplotlib.pyplot as plt
import pandas as pd
import argparse

# Create the parser
parser = argparse.ArgumentParser(description="Example of optional command-line arguments with defaults.")

# Add optional arguments with default values
parser.add_argument(
    "--token_history",
    type=str,
    default="TH_1.json",
    help="Path to a token history file (default: TH_1.json)"
)
parser.add_argument(
    "--reaction_history",
    type=str,
    default="RAH_1.json",
    help="Path to a reaction history file (default: RAH_1.json)"
)

args = parser.parse_args()



d = None
with open(args.token_history) as f:
  d = json.load(f)

# This determines how many datapoints to show
hist_cutoff = 100
colors = plt.get_cmap('tab20', 20)

def plot_token_history(d, title, cap = None):
  """
  d
  title
  cap: Because the tokens might be on different chains you can specify a cap,
  so the amount of tokens will only be plotted upto the cap.
  """
  for i, (key, y_values) in enumerate(d.items()):
      if key == "13dpg_c":
         pass 
      y_values = y_values[0:hist_cutoff]
      y_values = [cap if cap is not None and y > cap else y for y in y_values]
      x_values = range(len(y_values))  # Use the index as the x-values
  
      # Use a different color from the palette for each key
      #plt.scatter(x_values, y_values,  label=key, color=colors(i % 20), s=10)
      plt.plot(x_values, y_values, label=key, color=colors(i % 20), linestyle='-')
  
  # Add labels and legend
  plt.xlabel('Index (x-axis)')
  plt.ylabel('Values (y-axis)')
  plt.legend()
  plt.title(title)
  plt.show()



plot_token_history(d, "All metabolites", cap = 20)
#
#decresing_metabolites = {k: v for k, v in d.items() if v[0] > v[len(v) - 1] }
#plot_token_history(decresing_metabolites, "Decreasing Metabolites")
#
#
#increasing_metabolites = {k: v for k, v in d.items() if v[0] < v[len(v) - 1] }
#plot_token_history(increasing_metabolites, "Increasing Metabolites")


#def plot_zero_at_t(d, t):
#  return {k: v for k, v in d.items() if v[t] == 0 }
#  
##
#z = plot_zero_at_t(d, C-1)
#print(z.keys())
#plot_token_history(z, f'Zero at {C-1}')

reaction_history = None
with open(args.reaction_history) as f:
  reaction_history = json.load(f)

def plot_reaction_activity(reaction_history):
  # Corresponding time or index values (x-axis)
  # To figure out how many entries we have per metabolite
  # We just get the first list in the json file and determine its length
  first_key = list(reaction_history.keys())[0]
  # We then generate a range from 0 to this length
  history_range = range(len(reaction_history[first_key]))

  if (len(history_range) < hist_cutoff):
    raise Exception("hist_cutoff is too large, there is less data then the cutoff")

  history_range = history_range[0:hist_cutoff]
  # Create the figure,
  amount_metabolites = len(reaction_history) 
  _, axs = plt.subplots(amount_metabolites, 1, sharex=True, figsize=(6, 6))
  c = 0
  # Plot each boolean vector in its own subplot
  for i, (key, vec) in enumerate(reaction_history.items()):
    c += 1
    if c == amount_metabolites:
      break
    v2 = list(map(int, vec[0:hist_cutoff]))
    vec2 = pd.DataFrame(v2).rolling(window=1).sum()
    axs[i].plot(history_range[0:hist_cutoff], vec2[0:hist_cutoff], label=key)
    axs[i].set_ylabel(key)
    #axs[i].set_ylim(-0.2, 1.2)  # Adjust y-limits for boolean data (True/False)
    axs[i].set_ylabel('', rotation='horizontal', fontsize=20, labelpad=20)
    axs[i].tick_params(axis='y')
    axs[i].legend(loc='upper right')
  
  # Set common x-label
  axs[-1].set_xlabel('Index')
  
  # Adjust the layout to avoid overlap
  plt.tight_layout()
  
  # Show plot
  plt.suptitle('Boolean Vectors with Separate Y-Axes Below', y=1.05)
  plt.show()

plot_reaction_activity(reaction_history)
