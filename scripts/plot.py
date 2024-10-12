import json
import matplotlib.pyplot as plt
import pandas as pd
d = None
with open('TH_1.json') as f:
  d = json.load(f)

#d = {"ATP": d["ATP"]}
C = 200

# Create scatter plots for each key
colors = plt.get_cmap('tab20', 20)

def plot_token_history(d, title, cap = None):
  for i, (key, y_values) in enumerate(d.items()):
      if key == "13dpg_c":
         pass 
      y_values = y_values[0:C]
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


#
#plot_token_history(d, "All metabolites", cap = 20)
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

bools = None
with open('RAH_1.json') as f:
  bools = json.load(f)

# Corresponding time or index values (x-axis)
x = range(len(bools[list(bools.keys())[0]]))
x = x[0:C]
# Create the figure
X = len(bools) - 30
fig, axs = plt.subplots(X, 1, sharex=True, figsize=(6, 6))
c = 0
# Plot each boolean vector in its own subplot
for i, (key, vec) in enumerate(bools.items()):
  c += 1
  if c == X:
    break
  v2 = list(map(int, vec[0:C]))
  vec2 = pd.DataFrame(v2).rolling(window=30).sum()
  axs[i].plot(x[0:C], vec2[0:C], label=key)
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
