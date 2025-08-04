#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <cassert>


// ----------- Zipf Distribution Sampling ----------------
std::vector<int> sample_zipf(int domain_size, int total_count, double alpha, std::mt19937& rng) {
    std::vector<double> probs(domain_size);
    for (int i = 0; i < domain_size; ++i)
        probs[i] = 1.0 / pow(i + 1, alpha);
    double Z = accumulate(probs.begin(), probs.end(), 0.0);
    for (auto& p : probs) p /= Z;

    std::discrete_distribution<int> dist(probs.begin(), probs.end());

    std::vector<int> histogram(domain_size, 0);
    for (int i = 0; i < total_count; ++i)
        histogram[dist(rng)]++;
    return histogram;
}

// ----------- k-RR Mechanism ----------------
struct kRR {
    double epsilon;
    int k;
    double p, q;

    kRR(double eps, int domain_size) : epsilon(eps), k(domain_size) {
        p = exp(eps) / (exp(eps) + k - 1);
        q = 1.0 / (exp(eps) + k - 1);
    }

    std::vector<std::vector<double>> get_channel() {
        std::vector<std::vector<double>> A(k, std::vector<double>(k, q));
        for (int i = 0; i < k; ++i) A[i][i] = p;
        return A;
    }

    std::vector<int> perturb(const std::vector<int>& true_hist, std::mt19937& rng) {
        std::vector<int> noisy_hist(k, 0);
        std::vector<std::vector<double>> A = get_channel();

        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < true_hist[i]; ++j) {
                std::discrete_distribution<int> d(A[i].begin(), A[i].end());
                int reported = d(rng);
                noisy_hist[reported]++;
            }
        }
        return noisy_hist;
    }
};

// ----------- MLE (closed-form for k-RR) ----------------
std::vector<double> MLE(const std::vector<double>& phi_input_v, double p) {
	std::vector<double> phi_v = phi_input_v;                // make a copy
    int n = phi_v.size(); 					// domain size
    double q = (1.0 - p) / (n - 1);			// the low probability of the kRR

    // Check for valid p and q
    if (!(p >= q && q >= 0)) {
        throw std::invalid_argument("Invalid noise parameters: p = " + std::to_string(p) + ", q = " + std::to_string(q));
    }

    // Create argsort (indices that would sort the vector)
    std::vector<int> sig(n);
    std::iota(sig.begin(), sig.end(), 0); // fill with 0,1,...,n-1
    std::sort(sig.begin(), sig.end(), [&phi_v](int i, int j) {
        return phi_v[i] < phi_v[j];
    });

    int k = 0;
    double s = std::accumulate(phi_v.begin(), phi_v.end(), 0.0);
    while (k < n && q * s > (1.0 - k * q) * phi_v[sig[k]]) {
        s -= phi_v[sig[k]];
        k++;
    }

    std::vector<double> theta(n, 0.0); 
    // Compute denominator only once to avoid division by zero
    if (s > 0 && (p - q) != 0.0) {
        for (int i = k; i < n; ++i) {
            int idx = sig[i];
            theta[idx] = ((1.0 - k * q) * phi_v[idx] - s * q) / (s * (p - q));
        }
    }

    return theta;
}

// ----------- IBU ----------------
std::vector<double> run_ibu(
    const std::vector<double>& phi, const std::vector<std::vector<double>>& A, const std::vector<double> mle,
    int iterations, std::vector<double>& distances) {
    int k = phi.size();
    //int n = accumulate(y.begin(), y.end(), 0);
    std::vector<double> pr_x(k, 1.0/k);
    double dist = 0.0;
    for (int i = 0; i < k; ++i)
        dist += (pr_x[i] - mle[i])*(pr_x[i] - mle[i]);  // compute the squared distance
    distances.push_back(dist);
    
    for (int t = 1; t < iterations; ++t) {
        std::vector<double> pr_z(k , 0.0);
        for (int z=0; z<k; z++) {
            for (int x=0; x<k; x++)
                pr_z[z] = pr_z[z] + pr_x[x] * A[x][z];
        }

        std::vector<double> new_pr_x(k,0.0);
        for (int x=0; x<k; x++) {
            for (int z=0; z<k; z++) 
                new_pr_x[x] = new_pr_x[x] + (phi[z] * pr_x[x] * A[x][z])/pr_z[z];
        }
        pr_x = new_pr_x;
        dist = 0.0;
        for (int i = 0; i < k; ++i)
            dist += (pr_x[i] - mle[i])*(pr_x[i] - mle[i]);  // compute the squared distance
        distances.push_back(dist);
    }
    return pr_x;
}

// ----------- Save/Print Utilities ----------------
void save_vector(const std::string& path, const std::vector<double>& v) {
    std::ofstream f(path);
    for (auto x : v) f << x << "\n";
}

void save_distances(const std::string& path, const std::vector<double>& v) {
    std::ofstream f(path);
    //f << "# Iteration\tL1 Distance\n";
    for (size_t i = 0; i < v.size(); ++i)
        f << i << "\t" << v[i] << "\n";
}

std::string to_string_fixed(double value, int precision = 2) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}


// ----------- Main ----------------
int main(int argc, char* argv[]) {

    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <int_k> <double_s> <double_priv> <int_n> \n";
        return 1;
    }

    int k = std::atoi(argv[1]);           // domain size: 500
    double alpha = std::atof(argv[2]);    // Zipf parameter: 0.01, 1.3 
    double eps = std::atof(argv[3]);      // privacy parameter: 0.5, 1 , 2
    int n = std::atoi(argv[4]);           // number of users: 10000, 500000

    int iterations = 40000;

    std::mt19937 rng(1234); // for reproducibility

    // Generate Zipf Data
    std::vector<int> x = sample_zipf(k, n, alpha, rng);
    std::vector<double> xnorm(k); // original distribution
    for (int i = 0; i < k; ++i) xnorm[i] = x[i] / static_cast<double>(n);

    // Apply k-RR
    kRR mechanism(eps, k);
    std::vector<std::vector<double>> A = mechanism.get_channel();
    std::vector<int> y = mechanism.perturb(x, rng);  // noisy histogram

    int total = std::accumulate(y.begin(), y.end(), 0);
    std::vector<double> phi(y.size());  // noisy distribution
    for (size_t i = 0; i < y.size(); ++i)
        phi[i] = static_cast<double>(y[i]) / total;

    // MLE* Estimate
    std::vector<double> mle = MLE(phi, mechanism.p);

    // IBU
    std::vector<double> ibu_distances;
    std::vector<double> ibu = run_ibu(phi, A, mle, iterations, ibu_distances);

    // Output results
    std::string original_distribution = 
	   		"original_k_" 
	   		+ std::to_string(k) +
	   		"_s_" + to_string_fixed(alpha,2) +
	   		"_n_" + std::to_string(n) +
	   		".txt";
	
    std::string distance_ibu_file = 
	        "ibu_to_mle_k_" 
	        + std::to_string(k) +
	        "_s_" + to_string_fixed(alpha,2) +
	        "_priv_"+ to_string_fixed(eps,1) +
	        "_n_" + std::to_string(n) +
	        ".txt";

    
    // save_vector(original_distribution, xnorm);  // if needed
    // save_vector("mle_expression.txt", mle);     // if needed
    // save_vector("ibu_estimate.txt", ibu);       // if needed
    save_distances(distance_ibu_file, ibu_distances);

    return 0;
}




