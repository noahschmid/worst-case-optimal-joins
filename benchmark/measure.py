def run_program(name):
    x = []
    y = []
    for i in range(100, 1600, 100):
        output = os.popen(f".\\2\{name} {i}").read()
        numCycles = int(output.split("cycles:")[1].split(" ")[1].split(".")[0])
        flopsPerCyc = (i**2 + 2*(i**3))/numCycles
        x.append(i)
        y.append(flopsPerCyc)
        print(x)
        print(y)
        print(numCycles)

    return (x, y)