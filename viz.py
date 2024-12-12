import pandas as pd
import numpy as np
import plotly.express as px

# Load the dataset to viz
original_file_path = 'output_viz.csv'  
viz_df = pd.read_csv("output_viz.csv")

print(f"File {original_file_path} loaded into dataframe.")

print(viz_df.head(100))
print(viz_df.columns)

# Plot the data showing the total student enrollment
px.histogram(viz_df, x="count", y="traversal")
