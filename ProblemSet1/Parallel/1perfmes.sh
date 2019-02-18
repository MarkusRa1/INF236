#!/bin/bash 

#module load mpi/openmpi-x86_64 
mpicc -std=c11 ./Cellular1D-Parallel.c -O3 -o 1parallel.o 
mpicc -std=c11 ./Cellular2D-Parallel.c -O3 -o 2parallel.o 
rm output/results1D.txt 
rm output/results2D.txt 
echo Running parallel 1D automata 

k=( 10 11 12 13 14 15 16 17 18 19 20 ) 
N=( 1024 2048 4096 8192 16384 32768 ) # 65536 )
processes=( 1 2 4 8 16 32 64 128 ) # 256 512 ) 

printf "%15s" "${processes[@]}" >> output/results1D.txt 

# for i in "${k[@]}" 
# do 
#     result=() 
#     for p in "${processes[@]}" 
#     do 
#         result=("${result[@]}" $(mpirun -np "$p" ./1parallel.o mod2.txt k"$i".txt 1000)) 
#     done 
#     printf "\n $i" >> output/results1D.txt 
#     printf "%15s" "${result[@]}" >> output/results1D.txt 
# done 

echo Running parallel 2D automata
printf "%15s" "${processes[@]}" >> output/results2D.txt 
for i in "${N[@]}" 
do 
    result=() 
    for p in "${processes[@]}" 
    do 
        result=("${result[@]}" $(mpirun -np "$p" ./2parallel.o gameoflife.txt "$i" 10)) 
    done 
    printf "\n $i" >> output/results2D.txt 
    printf "%15s" "${result[@]}" >> output/results2D.txt 
done