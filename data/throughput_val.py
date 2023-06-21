import os
cycles = []
std_dev = []
duration = []
size = []
for i in range(1, 40):
    os.system(f"python3.10 data/create_datasets.py {i}")
    res = os.popen(f"python3.10 benchmark/e2e_measure.py").read()

    f = open("data/int_res.txt", "w")
    cycles.append(res.split("cycles:")[1].split(" ")[1])
    duration.append(res.split("duration:")[1].split(" ")[1])
    std_dev.append(res.split("stddev:")[1].split(" ")[1])
    size.append(i)
    f.write("cycles: ")
    f.write(str(cycles))
    f.write("\n")
    f.write("duration: ")
    f.write(str(duration))
    f.write("\n")
    f.write("stddev: ")
    f.write(str(std_dev))
    f.write("\n")
    f.write("size: ")
    f.write(str(size))
    f.write("\n")
    f.close()
    print(i)
print(cycles)
print(std_dev)
print(duration)