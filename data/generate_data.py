import random

# Define the number of rows and columns
num_rows = 101
num_cols = 5

# Define the filename and open the file for writing
filename = f'data_{num_cols}_col.tsv'
with open(filename, "w") as f:

    # Write the header row
    header = "v1 v2 v3 v4 v5\n"
    f.write(header)

    # Write the random data
    for i in range(num_rows - 1):
        row_data = [str(random.randint(1, 10)) for j in range(num_cols)]
        row = " ".join(row_data) + "\n"
        f.write(row)