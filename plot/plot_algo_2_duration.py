import matplotlib.pyplot as plt
import numpy as np



num_cycles_2_col = [4.1e-05, 8.2e-05, 0.000123, 0.000164, 0.000203, 0.000243, 0.000284, 0.000323, 0.000361, 0.000404, 0.00044, 0.000483, 0.000519, 0.000559, 0.000606]
num_cycles_3_col = [5.8e-05, 0.000113, 0.000172, 0.000226, 0.000281, 0.000333, 0.000392, 0.000443, 0.0005, 0.000555, 0.000635, 0.000662, 0.000719, 0.000767, 0.000828]

num_cycles_4_col = [7.4e-05, 0.000145, 0.000215, 0.000294, 0.000356, 0.000427, 0.000496, 0.000569, 0.000635, 0.000705, 0.000773, 0.000849, 0.000912, 0.000988, 0.001065]
num_cycles_5_col = [9.2e-05, 0.000178, 0.000268, 0.000355, 0.00044, 0.000525, 0.000613, 0.000694, 0.000778, 0.000864, 0.00095, 0.001042, 0.001127, 0.001208, 0.001299]
num_cycles_6_col = [0.000109, 0.000211, 0.000318, 0.000423, 0.000523, 0.000628, 0.00073, 0.000828, 0.00093, 0.001037, 0.001139, 0.001232, 0.001338, 0.001443, 0.001538]


x = np.arange(100, 1600, 100)

# plot
fig, ax = plt.subplots()

ax.plot(x, num_cycles_6_col, 'o-', label="6 columns", color='#00008b')
ax.plot(x, num_cycles_5_col, 'o-', label="5 columns", color='#800020')
ax.plot(x, num_cycles_4_col, 'o-', label="4 columns", color='#006269')
ax.plot(x, num_cycles_3_col, 'o-', label="3 columns", color='purple')
ax.plot(x, num_cycles_2_col, 'o-', label="2 columns", color='darkorange')

plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))


ax.grid(axis='y', color="white", linestyle='-', linewidth=1)
ax.set_facecolor("gainsboro")
ax.set_xlabel("# rows ")
ax.set_ylabel("seconds", rotation=1)
ax.yaxis.set_label_coords(0.05, 1.02)


ax.set_title("Algorithm 2", y=1.06, weight='bold')

plt.xticks(range(100, 1600, 200))
#plt.yticks(range(0, 3, 1))
fig.savefig('plot_algo_2_duration.svg', bbox_inches='tight')
plt.show()