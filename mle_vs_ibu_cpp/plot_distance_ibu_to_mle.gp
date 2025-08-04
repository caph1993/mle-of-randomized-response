# This plots the distance from an ibu estimate to the mle expression

set terminal pdf linewidth 2  
set lmargin 11      

# Check if the right number of arguments is provided
if (ARGC != 3) {
    print ARGC   
    print "Usage: gnuplot -c plot_distance_ibu_to_mle.gp <k> <s> <n>"
    exit
}

k = ARG1 # k size
s = ARG2 # zipf concentration
n = ARG3 # number of samples
distType = ARG4 # distance type, e.g. TV or SQD


priv_a = "0.5"  # epsilon
priv_b = "1.0"  # epsilon
priv_c = "2.0"  # epsilon

set title font ",20" "k = ".k.", s = ".s. ", n = ".n

results_folder = "./"

set output "ibu_to_mle_k_".k."_s_".s."_n_".n.".pdf"

data_a = results_folder."/ibu_to_mle_k_".k."_s_".s."_priv_".priv_a."_n_".n.".txt"
data_b = results_folder."/ibu_to_mle_k_".k."_s_".s."_priv_".priv_b."_n_".n.".txt"
data_c = results_folder."/ibu_to_mle_k_".k."_s_".s."_priv_".priv_c."_n_".n.".txt"

set key font ",20"
set grid

set xtics font ",20"
set xrange [0:40]  # This is the number of kilo iterations. Accordingly, the iteration counts in the files must be scaled down in the plot command 
set xlabel font ",20" "iteration (×1000)"
set ylabel font ",20" "Squared Error" offset -2,0
set ytics font ",20"
set yrange [0:0.12]

plot   data_a using ($1/1000):2 with lines lt 1 lw 2 title '{/Symbol e} ='.priv_a ,\
       data_b using ($1/1000):2 with lines lt 4 lw 2 title '{/Symbol e} ='.priv_b ,\
       data_c using ($1/1000):2 with lines lt 2 lw 2 title '{/Symbol e} ='.priv_c \

