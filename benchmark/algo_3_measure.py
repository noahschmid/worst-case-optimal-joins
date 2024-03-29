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
    list_stddev = []


    # list_of_commands = [("2 ./data/imdb_data/title_basics.tsv ./data/imdb_data/title_crew.tsv tconst"),
    #                 ("2 ./data/imdb_data/title_basics.tsv ./data/imdb_data/title_episode.tsv tconst"),
    #                 ("2 ./data/imdb_data/title_basics.tsv ./data/imdb_data/title_ratings.tsv tconst"),
    #                 ("2 ./data/imdb_data/title_crew.tsv ./data/imdb_data/title_episode.tsv tconst"),
    #                 ("2 ./data/imdb_data/title_crew.tsv ./data/imdb_data/title_ratings.tsv tconst"),
    #                 ("2 ./data/imdb_data/title_episode.tsv ./data/imdb_data/title_ratings.tsv tconst"),
    #                 # faster ones, less cycles
    #                 ("2 ./data/imdb_data/title_basics.tsv ./data/imdb_data/title_principals.tsv tconst"),
    #                 ("2 ./data/imdb_data/title_episode.tsv ./data/imdb_data/title_principals.tsv tconst"),
    #                     ]

    times = 1
    mini_data_cmd = f"3 ./data/large_r1.tsv ./data/large_r2.tsv ./data/large_r3.tsv v1 v2 v3"
    for _ in range(times):
        cycles, dur, stddev = run_program("bm3.exe", mini_data_cmd)
        list_num_cycles.append(cycles)
        list_duration.append(dur)
        list_stddev.append(stddev)


    # print the average of the cycles and duration
    cycles = sum(list_num_cycles) / len(list_num_cycles)
    dur = sum(list_duration) / len(list_duration)
    stddev = sum(list_stddev) / len(list_stddev)
    print(f"cycles: {cycles}")
    print(f"duration: {dur}")
    print(f"stddev: {stddev}")

if __name__ == "__main__":
    main()