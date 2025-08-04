import numpy as np

n_repeats = 10 # number of seeds to repeat each experiment
pop_eps = np.arange(1, 10.1, 1) # Epsilon values to vary
pop_N = np.array([int(10**j) for j in range(2, 7)]) # Population sizes to vary
pop_K = np.array([50, 100, 1_000, 5_000, 10_000])#, 50_000]) # Domain sizes to vary
pop_s = np.array([0.01, 1.3, 2.5]) # 0.25, 0.8, 5,  # Zipf exponents to vary: 0.01~uniform vs 5~spike