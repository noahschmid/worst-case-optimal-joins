import math
import os

def run_program(name: str, cmd: str):
    # call the program with the given number of columns and rows
    # and get the output, use windows and linux compatible path
    path = os.path.join(os.getcwd(), name)
    output = os.popen(f"{path} {cmd}").read()

    num_cycles = int(output.split("cycles:")[1].split(" ")[1].split(".")[0])
    duration = float(output.split("duration:")[1].split(" ")[1])
    stddev = float(output.split("stddev:")[1].split(" ")[1].split(" ")[0])

    return num_cycles, duration, stddev


def main():
    list_num_cycles = []
    list_duration = []

    times = 3
    mini_data_cmd = f"3 ./data/large_r1.tsv ./data/large_r2.tsv ./data/large_r3.tsv v1 v2 v3"
    for _ in range(times):
        cycles, dur, stddev = run_program("./e2e.exe", mini_data_cmd)
        list_num_cycles.append(cycles)
        list_duration.append(dur)


    # print the average of the cycles and duration
    cycles = sum(list_num_cycles) / len(list_num_cycles)
    dur = sum(list_duration) / len(list_duration)

    std = 0
    # calculate standard deviation */
    for i in range(times):
        std += pow((list_num_cycles[i] - cycles), 2);

    std /= times;
    std = math.sqrt(std);

    print(f"cycles: {cycles}")
    print(f"duration: {dur}")
    print(f"stddev: {std}")

if __name__ == "__main__":
    main()