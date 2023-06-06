import matplotlib.pyplot as plt
import numpy as np

font = {'family': 'serif',
        'size': 18}

plt.rc('font', **font)

num_cycles = [25436191.4, 25357306.6, 25141660.0, 24959876.0, 23077487.2, 16355951.4, 4412899.8, 2230884.2, 2197449.0, 2185136.4]




x = np.arange(1, 11, 1)

# plot
fig, ax = plt.subplots()

ax.plot(x, num_cycles, 'o-', color='#800020')
# ax.plot(x, num_cycles_o2, 'o-', label="-O2", color='#800020')


labels = ['baseline', 'Opt1', 'Opt2', 'Opt3', 'Opt4', 'Opt5', 'Opt6', 'Opt7', 'Opt8', 'Opt9']




ax.grid(axis='y', color="white", linestyle='-', linewidth=1)
ax.set_facecolor("gainsboro")
ax.set_xlabel("Version")
ax.set_ylabel("# cycles", rotation=1)
ax.yaxis.set_label_coords(-0.03, 1.02)


ax.set_title("Worst-case optimal join", y=1.06, weight='bold')


plt.xticks(range(1, 11, 1), labels=labels, rotation=45)



fig.savefig('plot_e2e_cycles.svg', bbox_inches='tight')
plt.show()