import numpy as np


# ==== Metrics ====
def log_likelihood(obs, est, p_truth):
    "scaled log prob of observing obs given params=est"
    obs = np.asarray(obs)
    est = np.asarray(est)
    if np.any(est < 0):
        return -np.inf
    n = len(est)
    p = p_truth
    q = (1 - p) / (n - 1)
    return np.sum(obs * np.log((p - q) * est + q))


# ==== Data Generation ====
def generate_synthetic_data(
                            n: int,
                            k: int,
                            s: float = 1.3,
                            seed: int = 42
                        ) -> np.ndarray:
    """
    Parameters
    ----------
    n : int
        Number of samples.
    k : int
        Domain size.
    s : float, optional
        Shape parameter for "zipf".
    seed : int, optional
        Random seed for reproducibility.

    Returns
    -------
    np.ndarray
        Array of sampled values.
    """

    if s < 5: # Zipf distribution

        if s <= 0:
            raise ValueError("Parameter 's' must be > 0 for Zipf distribution.")
        
        np.random.seed(seed)
        weights = 1.0 / np.arange(1, k + 1) ** s
        weights /= weights.sum()
        samples = np.random.choice(np.arange(k), size=n, p=weights)
        return samples

    else: # Spike distribution

        spike_index = 0 # all samples are the same value
        return np.full(n, spike_index, dtype=int)