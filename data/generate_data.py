import random
import argparse


parser = argparse.ArgumentParser(description="Generate random data.")
parser.add_argument("num_rows", type=int, help="Number of rows")
parser.add_argument("num_cols", type=int, help="Number of columns")
parser.add_argument("range_start", type=int, help="Start of the range")
parser.add_argument("range_end", type=int, help="End of the range")
args = parser.parse_args()

num_rows = args.num_rows
num_cols = args.num_cols
range_start = args.range_start
range_end = args.range_end


filename = f'data_{num_cols}_col_{num_rows}_row.tsv'

with open(filename, "w") as f:
    # Write the header row
    header = " ".join(f"v{i+1}" for i in range(num_cols)) + "\n"
    f.write(header)

    # Write the random data
    for _ in range(num_rows):
        row_data = [str(random.randint(range_start, range_end)) for _ in range(num_cols)]
        row = " ".join(row_data) + "\n"
        f.write(row)