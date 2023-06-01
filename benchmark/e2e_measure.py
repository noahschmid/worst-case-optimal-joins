import os

def run_program(name: str, cmd: str):
    # call the program with the given number of columns and rows
    # and get the output, use windows and linux compatible path
    path = os.path.join(os.getcwd(), name)
    output = os.popen(f"{path} {cmd}").read()

    num_cycles = int(output.split("RDTSC instruction:")[1].split(" ")[1].split(".")[0])
    duration = float(output.split("timing:")[1].split(" ")[1])

    return num_cycles, duration


def main():
    list_num_cycles = []
    list_duration = []



    mini_data_cmd = f"3 ./data/mini_mini_data_1.tsv ./data/mini_mini_data_2.tsv ./data/mini_mini_data_3.tsv v1 v2 v3"
    cycles, dur = run_program("e2e.exe", mini_data_cmd)
    list_num_cycles.append(cycles)
    list_duration.append(dur)

    print(list_num_cycles)
    print(list_duration)

if __name__ == "__main__":
    main()