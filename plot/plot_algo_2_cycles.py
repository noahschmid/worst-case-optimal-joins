import matplotlib.pyplot as plt
import numpy as np



num_cycles_2_col = [118057, 229692, 346335, 458544, 568609, 687629, 792671, 906077, 1009746, 1132663, 1231870, 1350894, 1457870, 1565496, 1684648]
num_cycles_3_col = [164210, 317397, 477834, 636046, 791198, 932909, 1097266, 1244378, 1404092, 1557206, 1698983, 1894504, 2006644, 2147682, 2319796]
num_cycles_4_col = [211458, 408386, 603801, 802331, 1036133, 1198107, 1387815, 1583372, 1856378, 2009691, 2165157, 2380303, 2556688, 2777421, 2999669]
num_cycles_5_col = [257714, 500984, 752660, 995665, 1230626, 1476363, 1720741, 1943484, 2177684, 2421661, 2661118, 2918826, 3160986, 3384068, 3640322]
num_cycles_6_col = [306176, 592355, 891041, 1179753, 1465113, 1761986, 2033084, 2308027, 2606195, 2906289, 3190244, 3452967, 3744822, 4049636, 4312359]


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
ax.set_ylabel("# cycles", rotation=1)
ax.yaxis.set_label_coords(0.05, 1.02)


ax.set_title("Algorithm 2", y=1.06, weight='bold')

plt.xticks(range(100, 1600, 200))
#plt.yticks(range(0, 3, 1))
fig.savefig('plot_algo_2_cycles.svg', bbox_inches='tight')
plt.show()