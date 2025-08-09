import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV
df = pd.read_csv("timings.csv")

# Group and average over repetitions
avg_df = df.groupby(["VECTORIZE", "N", "type"], as_index=False)["time"].mean()

# Order for X-axis categories
order = [
    ("disabled", "naive"),
    ("disabled", "cached"),
    ("enabled", "naive"),
    ("enabled", "cached")
]

# Create separate plots for each N with individual y-axis scaling
for N_value in sorted(df["N"].unique()):
    subset = avg_df[avg_df["N"] == N_value]
    
    # Extract values in the specified order
    times = []
    labels = []
    for vec, t in order:
        row = subset[(subset["VECTORIZE"] == vec) & (subset["type"] == t)]
        if not row.empty:
            times.append(row["time"].values[0])
            labels.append(f"{vec}-{t}")
        else:
            times.append(None)
            labels.append(f"{vec}-{t}")
    
    # Plot (auto y-scale per N)
    plt.figure()
    plt.bar(labels, times, color=["red", "orange", "blue", "green"])
    plt.ylabel("Average Time (s)")
    plt.title(f"Matrix Multiplication Timings (N={N_value})")
    plt.grid(axis="y", linestyle="--", alpha=0.7)
    plt.tight_layout()
    plt.savefig(f"timings_N{N_value}.png")  # Save each plot as PNG
    plt.close()

