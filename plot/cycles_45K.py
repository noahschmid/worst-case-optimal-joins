import matplotlib
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure


ax = plt.subplot(111)
font = {'family': 'normal', 'weight' : 'normal','size': 18}
plt.rc('font', **font)

figure(figsize=(8, 6), dpi=120)
ax = plt.subplot(111)



# Sample data
x = np.arange(0, 10)  # Optimization version
y = np.array([32851135232.0, 32562003143.333332, 31480215936.333332, 28697998081.333332, 10136158399.333334, 10701929915.333334, 10336071583.666666, 10215792024.666666, 10139575204.0, 8837682330.0])  # #cycles
std_dev = np.array([2132865907.438595, 2947337010.1086183, 980622479.7414739, 1863263841.4707274, 96996692.90120862, 1069644774.5581222, 858914401.0975219, 1499203408.2888324, 1211348454.014225, 171279512.57958955])  # Standard deviation

# Line plot
ax.plot(x, y, marker='o', linestyle='-', color='b')

# Fill between lines (Standard deviation)
ax.fill_between(x, y - std_dev, y + std_dev, alpha=0.2, color='b')

# Setting x-ticks to show all values individually
ax.set_xticks(x)

# Set plot title and labels
ax.set_title('Runtime Comparison (45K Rows)')
ax.set_xlabel('version', fontsize=20)
ax.set_ylabel('cycles', fontsize=20, rotation=1)
ax.yaxis.set_label_coords(-0.1, 1.02)


ax.xaxis.set_tick_params(labelsize=16)
ax.yaxis.set_tick_params(labelsize=16)

mkfunc = lambda x, pos: '%1dB' % (x * 1e-9) if x >= 1e6 else '%1dK' % (x * 1e-3) if x >= 1e3 else '%1d' % x
mkformatter = matplotlib.ticker.FuncFormatter(mkfunc)
ax.yaxis.set_major_formatter(mkformatter)




plt.savefig("cycles_45K.svg")
# Show the plot
plt.show()