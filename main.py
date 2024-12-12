import pandas as pd
import numpy as np
import re

# Load Sydney Trains dataset
sydney_trains_file_path = 'sydneytrains.csv'  
sydney_trains_data = pd.read_csv("datasets/sydneytrains.csv")

# Ensure relevant column 'route_long_name' exists
if 'route_long_name' in sydney_trains_data.columns:
    # Split the column into three new columns: origin, destination, and waypoint
    sydney_trains_data[['origin', 'destination', 'waypoint']] = sydney_trains_data['route_long_name'].str.extract(
        r'^(.*?)\s+to\s+(.*?)(?:\s+via\s+(.*))?$'
    )

    # Remove rows where 'route_long_name' is empty or null
    cleaned_sydney_trains_data = sydney_trains_data.dropna(subset=['route_long_name']).reset_index(drop=True)

    # Save the cleaned dataset to a new CSV
    cleaned_sydney_trains_data.to_csv('cleaned_sydney_trains.csv', index=False)

    # Create Edge Set
    edge_set = cleaned_sydney_trains_data[['origin', 'destination']].drop_duplicates()
    edge_set.to_csv('edge_set.csv', index=False)
    print("Edge set saved to 'edge_set.csv'.")

    # Create Adjacency Matrix
    # Map station names to indices
    stations = pd.concat([cleaned_sydney_trains_data['origin'], cleaned_sydney_trains_data['destination']]).unique()
    station_index = {station: i for i, station in enumerate(stations)}

    # Initialize adjacency matrix
    size = len(stations)
    adjacency_matrix = np.zeros((size, size), dtype=int)

    # Populate adjacency matrix
    for _, row in edge_set.iterrows():
        origin_idx = station_index[row['origin']]
        destination_idx = station_index[row['destination']]
        adjacency_matrix[origin_idx, destination_idx] = 1

    # Save adjacency matrix to CSV
    adjacency_matrix_df = pd.DataFrame(adjacency_matrix, index=stations, columns=stations)
    adjacency_matrix_df.to_csv('adjacency_matrix.csv', index=True)
    print("Adjacency matrix saved to 'adjacency_matrix.csv'.")

    print("All outputs generated successfully.")
else:
    print("Column 'route_long_name' not found in the dataset.")
