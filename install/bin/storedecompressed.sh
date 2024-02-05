#!/bin/bash

# Define the command
base_command_King_col="./cuSZp_gpu_f32_api ../../../../VDIs/Kingsnake_1920_1080_20_VDI_col_1 ABS"
base_command_King_depth="./cuSZp_gpu_f32_api ../../../../VDIs/Kingsnake_1920_1080_20_VDI_depth_1 ABS"
base_command_Ray_col="./cuSZp_gpu_f32_api ../../../../VDIs/Rayleigh_Taylor_1920_1080_20_VDI_col_1 ABS"
base_command_Ray_depth="./cuSZp_gpu_f32_api ../../../../VDIs/Rayleigh_Taylor_1920_1080_20_VDI_depth_1 ABS"
base_command_Rich_col="./cuSZp_gpu_f32_api ../../../../VDIs/Richtmyer_Meshkov_1920_1080_20_VDI_col_1 ABS"
base_command_Rich_depth="./cuSZp_gpu_f32_api ../../../../VDIs/Richtmyer_Meshkov_1920_1080_20_VDI_depth_1 ABS"


# Loop from 1 to 8
for i in {1..8}; do
    command="$base_command_King_col 1E-$i"
    echo "Executing: $command"
    $command

    command="$base_command_King_depth 1E-$i"
    echo "Executing: $command"
    $command

    command="$base_command_Ray_col 1E-$i"
    echo "Executing: $command"
    $command

    command="$base_command_Ray_depth 1E-$i"
    echo "Executing: $command"
    $command

    command="$base_command_Rich_col 1E-$i"
    echo "Executing: $command"
    $command

    command="$base_command_Rich_depth 1E-$i"
    echo "Executing: $command"
    $command

done

