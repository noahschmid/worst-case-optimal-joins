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
y = np.array([24838972.333333332, 24606480.333333332, 24490533.333333332, 24340361.666666668, 23213819.333333332, 16968704.0, 4633388.333333333, 2264274.6666666665, 2233682.3333333335, 2124561.3333333335])  # #cycles
std_dev = np.array([313710.22691384057, 133195.53677332186, 213089.54934799907, 51615.04600727711, 452068.0185639472, 505073.93298506306, 236041.84420211034, 53705.012804516984, 156722.16508947714, 12615.500003302815])  # Standard deviation

# Line plot
ax.plot(x, y, marker='o', linestyle='-', color='b')

# Fill between lines (Standard deviation)
ax.fill_between(x, y - std_dev, y + std_dev, alpha=0.2, color='b')

# Setting x-ticks to show all values individually
ax.set_xticks(x)

# Set plot title and labels
ax.set_title('Runtime Comparison (1K Rows)')
ax.set_xlabel('version', fontsize=20)
ax.set_ylabel('cycles', fontsize=20, rotation=1)
ax.yaxis.set_label_coords(-0.1, 1.02)


ax.xaxis.set_tick_params(labelsize=16)
ax.yaxis.set_tick_params(labelsize=16)

mkfunc = lambda x, pos: '%1dB' % (x * 1e-9) if x >= 1e6 else '%1dK' % (x * 1e-3) if x >= 1e3 else '%1d' % x
mkformatter = matplotlib.ticker.FuncFormatter(mkfunc)
ax.yaxis.set_major_formatter(mkformatter)




plt.savefig("cycles_1K.svg")
# Show the plot
plt.show()