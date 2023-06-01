import os

def parse_output(output: str):
    # return the number of cycles and the duration
    num_cycles = int(output.split("RDTSC instruction:")[1].split(" ")[1].split(".")[0])
    duration = float(output.split("timing:")[1].split(" ")[1])
    return num_cycles, duration

def run(cmd: str):
    # run the command and return the output
    return os.popen(cmd).read()

def run_program(name: str, num_cols: int):
    list_num_rows = []
    list_num_cycles = []
    list_duration = []

    for i in range(100, 1600, 100):
        # call the program with the given number of columns and rows
        # and get the output, use windows and linux compatible path
        path = os.path.join(os.getcwd(), name)
        output = run(f"{path} {num_cols} {i}")
        #output = os.popen(f".\\2\{name} {num_cols} {i}").read()


        num_cycles, duration = parse_output(output)

        list_num_rows.append(i)
        list_num_cycles.append(num_cycles)
        list_duration.append(duration)

    print(list_num_rows)
    print(list_num_cycles)
    print(list_duration)

def run_against_mini_data():
    table_paths = [
        './data/mini_mini_data_1.tsv',
        './data/mini_mini_data_2.tsv',
        './data/mini_mini_data_3.tsv',
    ]

    list_num_cycles = []
    list_duration = []
    for table_path in table_paths:
        cmd = f"./bm2.exe {table_path}"
        output = run(cmd)
        cycles, dur = parse_output(output)
        list_num_cycles.append(cycles)
        list_duration.append(dur)

    print(list_num_cycles)
    print(list_duration)


def main():

    # run the program for 2 to 6 columns
    # for i in range(2, 7):
    #     run_program("../bm2.exe", i)
    run_against_mini_data()




if __name__ == "__main__":
    main()