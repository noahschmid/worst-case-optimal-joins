import os

def run_program(name: str, num_cols: int):
    list_num_rows = []
    list_num_cycles = []
    list_duration = []

    for i in range(100, 1600, 100):
        # call the program with the given number of columns and rows
        # and get the output, use windows and linux compatible path
        path = os.path.join(os.getcwd(), name)
        output = os.popen(f"{path} {num_cols} {i}").read()
        #output = os.popen(f".\\2\{name} {num_cols} {i}").read()


        num_cycles = int(output.split("RDTSC instruction:")[1].split(" ")[1].split(".")[0])
        duration = float(output.split("timing:")[1].split(" ")[1])

        list_num_rows.append(i)
        list_num_cycles.append(num_cycles)
        list_duration.append(duration)

    print(list_num_rows)
    print(list_num_cycles)
    print(list_duration)

def main():

    # run the program for 2 to 6 columns
    for i in range(2, 7):
        run_program("../bm2.exe", i)


if __name__ == "__main__":
    main()