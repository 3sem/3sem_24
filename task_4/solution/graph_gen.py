import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
import glob
from matplotlib.ticker import AutoMinorLocator, MultipleLocator

# Directories for CSV files and saving graphs
data_dir = 'data'
output_dir = 'graphs'
os.makedirs(output_dir, exist_ok=True)

# Get all CSV files in the data directory
csv_files = glob.glob(os.path.join(data_dir, '*.csv'))

# Check that there are enough files for plotting
if len(csv_files) < 1:
    print("Необходимо минимум два CSV-файла для построения двух графиков.")
else:
    # Process the first two CSV files
    for i in range(1):
        file_path = csv_files[i]
        file_name = os.path.splitext(os.path.basename(file_path))[0]

        # Read data from CSV file
        data = pd.read_csv(file_path, sep=';', decimal=',')

        # Column labels
        y_label = "execution time, s"
        x_label = "threads"

        # Extracting values
        x_data = data[x_label]
        y_data = data[y_label]

        # Plot data points without linear approximation
        plt.plot(
            x_data, y_data, 'o', label=f'{file_name}', color=f'C{i}',
            markersize=2, markeredgewidth=1, markerfacecolor='none'
        )

    # Set up the axis with ticks and minor grid
    plt.gca().yaxis.set_major_locator(MultipleLocator(1))
    plt.gca().yaxis.set_minor_locator(AutoMinorLocator(5))
    plt.gca().xaxis.set_major_locator(MultipleLocator(10000))
    plt.gca().xaxis.set_minor_locator(AutoMinorLocator(5))

    plt.minorticks_on()
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)

    # Labels for the axes
    plt.xlabel('$'+x_label+'$')
    plt.ylabel('$'+y_label+'$')

    # Save the plot to a file
    output_file = f"{output_dir}/combined_graph.png"
    plt.savefig(output_file)
    plt.clf()  # Clear the figure for the next plot

    print(f"Graph saved as: {output_file}")
