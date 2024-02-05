#!/bin/bash

# Loop to execute the program 10 times
for index in {1..10}; do
    # Loop to execute the program with specific run values
    for run in 1 5 12; do
        # Execute the program with arguments and append the output to a file
        ./cuSZp_gpu_f32_api ../../../Kingsnake/Kingsnake_1920_1080_20_VDI_col_"$run" ABS 1E-"$index" >> "output_${index}.txt"
        ./cuSZp_gpu_f32_api ../../../Kingsnake/Kingsnake_1920_1080_20_VDI_depth_"$run" ABS 1E-"$index" >> "output_${index}.txt"

        #./cuSZp_gpu_f32_api ../../../Rayleigh_Taylor/Rayleigh_Taylor_1920_1080_20_VDI_col_"$run" ABS 1E-"$index" >> "output_${index}.txt"
        #./cuSZp_gpu_f32_api ../../../Rayleigh_Taylor/Rayleigh_Taylor_1920_1080_20_VDI_depth_"$run" ABS 1E-"$index" >> "output_${index}.txt"
    done
done

