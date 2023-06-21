import os

def parse_output(output: str):
    # return the number of cycles and the duration
    num_cycles = int(output.split("cycles:")[1].split(" ")[1].split(".")[0])
    duration = float(output.split("duration:")[1].split(" ")[1])
    stddev = float(output.split("stddev:")[1].split(" ")[1].split(" ")[0])
    return num_cycles, duration, stddev

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
    times = 1
    table_paths = [
        './data/large_r1.tsv',
        './data/large_r2.tsv',
        './data/large_r3.tsv',
    ]

    list_num_cycles = []
    list_duration = []
    list_stddev = []
    for table_path in table_paths:
        cmd = f"bm2.exe {table_path}"
        # run the program {times} times and get the average
        repetitive_cycles = []
        repetitive_durations = []
        repetitive_stddev = []
        for _ in range(times):
            output = run(cmd)
            single_cycles, single_dur, single_stddev = parse_output(output)
            repetitive_cycles.append(single_cycles)
            repetitive_durations.append(single_dur)
            repetitive_stddev.append(single_stddev)
        # calculate the average of the cycles and duration
        cycles = sum(repetitive_cycles) / len(repetitive_cycles)
        dur = sum(repetitive_durations) / len(repetitive_durations)
        stddev = sum(repetitive_stddev) / len(repetitive_stddev)
        list_num_cycles.append(cycles)
        list_duration.append(dur)
        list_stddev.append(stddev)

    print(list_num_cycles)
    print(list_duration)
    print(list_stddev)


def main():

    # run the program for 2 to 6 columns
    # for i in range(2, 7):
    #     run_program("../bm2.exe", i)
    run_against_mini_data()




if __name__ == "__main__":
    main()