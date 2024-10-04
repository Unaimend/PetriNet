import json
import matplotlib.pyplot as plt
d = None
with open('TH.json') as f:
  d = json.load(f)

print(d.keys())
#d = {"ATP": d["ATP"]}
C = 100

# Create scatter plots for each key
colors = plt.get_cmap('tab20', 20)

# Create scatter plots for each key
for i, (key, y_values) in enumerate(d.items()):
    if key == "13dpg_c":
        continue  # Skip this key if needed
    y_values = y_values[0:C]
    x_values = range(len(y_values))  # Use the index as the x-values

    # Use a different color from the palette for each key
    plt.scatter(x_values, y_values,  label=key, color=colors(i % 20), s=10)

# Add labels and legend
plt.xlabel('Index (x-axis)')
plt.ylabel('Values (y-axis)')
plt.legend()
plt.title('Scatter Plot for Each Group')
plt.show()


bools = None
with open('RAH.json') as f:
  bools = json.load(f)
print(bools.keys())
# Corresponding time or index values (x-axis)
x = range(len(bools[list(bools.keys())[0]]))
x = x[0:C]
# Create the figure

fig, axs = plt.subplots(len(bools), 1, sharex=True, figsize=(6, 6))

# Plot each boolean vector in its own subplot
for i, (key, vec) in enumerate(bools.items()):
    axs[i].plot(x[0:C], vec[0:C], label=key)
    axs[i].set_ylabel(key)
    axs[i].set_ylim(-0.2, 1.2)  # Adjust y-limits for boolean data (True/False)
    axs[i].set_ylabel('abc', rotation='horizontal', fontsize=20, labelpad=20)
    axs[i].tick_params(axis='y')
    axs[i].legend(loc='upper right')

# Set common x-label
axs[-1].set_xlabel('Index')

# Adjust the layout to avoid overlap
plt.tight_layout()

# Show plot
plt.suptitle('Boolean Vectors with Separate Y-Axes Below', y=1.05)
plt.show()
