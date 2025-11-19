# Estimating the True Distribution of Data Collected with Randomized Response

This is the public repository of the paper titled *Estimating the True Distribution of Data Collected with Randomized Response* which was accepted at AAAI 2026.

This repository contains:
- A [Python notebook](mle_vs_inv_python/MLE_RR_Experiments.ipynb) and a [C++ script](mle_vs_ibu_cpp/exec_script.sh) which can be used to reproduce all the experiments and figures in the paper.
- One [additional notebook](mle_vs_inv_python/MLE_RR_Experiments_update.ipynb) that decomposes MSE into bias and variance, as requested by several reviewers. This decomposition shows that the bias of MLE* seems to be bounded between that of INV-P and INV-N, just like the MSE.

# Usage

To replicate the plots of correctness of MLE* and convergence speed of IBU, follow the instructions in `mle_vs_ibu_cpp`.

To replicate the plots of comparison of MLE*, InvP and InvN follow the instructions in `mle_vs_inv_python`.



