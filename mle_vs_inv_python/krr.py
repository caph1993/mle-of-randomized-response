import numpy as np


# ==== RR ====
def grr_client_vec(true_vals, k: int, p: float, rng=None):
    """
    Vectorised Generalised Randomised Response (k-RR).

    Parameters
    ----------
    true_vals : 1-D array_like of ints in {0,…,k-1}
    k         : domain size  (k ≥ 2)
    p         : probability of truth (0 < p < 1)
    rng       : np.random.Generator, optional (for reproducibility)

    Returns
    -------
    reports   : ndarray of same shape as true_vals
    """
    true_vals = np.asarray(true_vals, dtype=np.int64)

    if rng is None:
        rng = np.random.default_rng()

    N = true_vals.size

    keep = rng.random(N) < p                             # boolean mask
    reports = true_vals.copy()

    n_flip = (~keep).sum()
    if n_flip:
        # draw ints in {0,…,k-2}, shift to avoid true value
        u = rng.integers(0, k - 1, size=n_flip, endpoint=False)
        t = true_vals[~keep]
        replacement = u + (u >= t)                       # bump when u ≥ t
        reports[~keep] = replacement

    return reports