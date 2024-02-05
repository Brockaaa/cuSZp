import os

def process_file(input_filename, output_filename):
    with open(input_filename, 'r') as input_file:
        # Read lines from the input file
        lines = input_file.readlines()

        # Initialize variables to store column sums
        sum_col1 = 0.0
        sum_col2 = 0.0
        sum_col3 = 0.0

        # Iterate over each line
        for line in lines:
            # Split the line into values
            values = [float(value) for value in line.strip().split(',')]

            # Add values to column sums
            sum_col1 += values[0]
            sum_col2 += values[1]
            sum_col3 += values[2]


        # Divide by specified factors
        sum_col1 /= 6
        sum_col2 /= 3
        sum_col3 /= 3

        # Create a processed file with the results
        with open(output_filename, 'w') as output_file:
            output_file.write(f"{sum_col1},{sum_col2},{sum_col3}\n")

def main():
    # Loop through indices from 1 to 10
    for index in range(1, 11):
        input_filename = f"output_{index}.txt"
        output_filename = f"processed_{index}.txt"

        # Check if the input file exists before processing
        if os.path.exists(input_filename):
            process_file(input_filename, output_filename)
            print(f"Processed {input_filename} -> {output_filename}")
        else:
            print(f"Input file {input_filename} not found.")

if __name__ == "__main__":
    main()
