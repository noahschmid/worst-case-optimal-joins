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

    
    list_of_commands = [("2 ./data/imdb_data/title_basics.tsv ./data/imdb_data/title_crew.tsv tconst"),
                    ("2 ./data/imdb_data/title_basics.tsv ./data/imdb_data/title_episode.tsv tconst"),
                    ("2 ./data/imdb_data/title_basics.tsv ./data/imdb_data/title_ratings.tsv tconst"),
                    ("2 ./data/imdb_data/title_crew.tsv ./data/imdb_data/title_episode.tsv tconst"),
                    ("2 ./data/imdb_data/title_crew.tsv ./data/imdb_data/title_ratings.tsv tconst"),
                    ("2 ./data/imdb_data/title_episode.tsv ./data/imdb_data/title_ratings.tsv tconst"),
                    # faster ones, less cycles
                    ("2 ./data/imdb_data/title_basics.tsv ./data/imdb_data/title_principals.tsv tconst"),
                    ("2 ./data/imdb_data/title_episode.tsv ./data/imdb_data/title_principals.tsv tconst"),
                        ]

    # run the program for 2 to 6 columns
    for cmd in list_of_commands:
        cycles, dur = run_program("bm3.exe", cmd)
        list_num_cycles.append(cycles)
        list_duration.append(dur)

    print(list_num_cycles)
    print(list_duration)

if __name__ == "__main__":
    main()