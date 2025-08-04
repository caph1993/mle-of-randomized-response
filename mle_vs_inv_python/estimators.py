import numpy as np


# ==== Estimators ====
def _INV(phi, p_truth):
    phi = np.asarray(phi)
    n = len(phi)
    p = p_truth
    q = (1 - p) / (n - 1)
    assert p_truth >= q and q >= 0, ("Invalid noise parameters", p_truth)#, a, b
    theta = (phi - q) / (p - q)
    return phi, n, p, q, theta

def INV(phi, p_truth):
    theta = _INV(phi, p_truth)[-1]
    return theta

def INV_P(phi, p_truth):
    phi, _, _, _, theta = _INV(phi, p_truth)
    thetas = np.sort(theta)
    sum_u = 0
    lambda_val = 0
    for j in range(1, len(thetas) + 1):
        u = thetas[-j]
        sum_u += u
        if u + (1 - sum_u) / j > 0:
            lambda_val = (1 - sum_u) / j
    theta = theta + lambda_val
    theta = np.maximum(theta, 0)
    return theta

def INV_N(phi, p_truth):
    theta = _INV(phi, p_truth)[-1]
    theta = np.maximum(theta, 0)
    theta /= np.sum(theta)
    return theta

def MLE(phi, p: float):
    phi = np.asarray(phi)
    n = len(phi)
    q = (1 - p) / (n - 1)
    assert p >= q and q >= 0, ("Invalid noise parameters", p, q)
    sig = np.argsort(phi)
    k = 0
    s = 1  # invariant: s = sum(phi[sig[k:]])
    while k < n and q * s > (1 - k * q) * phi[sig[k]]:
        s -= phi[sig[k]]
        k += 1
    theta = np.zeros(n)
    theta[sig[k:]] = ((1 - k * q) * phi[sig[k:]] - s * q) / (s * (p - q))
    return theta