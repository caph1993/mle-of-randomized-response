#!/bin/sh
set -e

# requires a preinstallation of C++ compiler and gnuplot plot utility. 
 
echo " compiling ..."
g++ -O2 -std=c++11 main.cpp -o bin

echo "running (producing data files) ..."

echo "producing data for: n = 500, s = 0.01, n = 10000"
./bin 500 0.01 0.5 10000
./bin 500 0.01 1.0 10000
./bin 500 0.01 2.0 10000

echo "producing data for: n = 500, s = 1.30, n = 10000"
./bin 500 1.30 0.5 10000
./bin 500 1.30 1.0 10000
./bin 500 1.30 2.0 10000

echo "producing data for: n = 500, s = 0.01, n = 500000"
./bin 500 0.01 0.5 500000
./bin 500 0.01 1.0 500000
./bin 500 0.01 2.0 500000

echo "producing data for: n = 500, s = 1.30, n = 500000"
./bin 500 1.30 0.5 500000
./bin 500 1.30 1.0 500000
./bin 500 1.30 2.0 500000

echo "producing the plots of the paper... " 
 
gnuplot -c plot_distance_ibu_to_mle.gp 500 0.01 10000 
gnuplot -c plot_distance_ibu_to_mle.gp 500 1.30 10000 
gnuplot -c plot_distance_ibu_to_mle.gp 500 0.01 500000
gnuplot -c plot_distance_ibu_to_mle.gp 500 1.30 500000

echo "finished"
