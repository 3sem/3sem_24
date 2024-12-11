#!/bin/bash

MAX_N_THREADS=24

for (( n_threads = 1; n_threads <= $MAX_N_THREADS; n_threads++ )) 
do
	echo $n_threads
	
	for (( i = 0; i < 5; i++ ))
	do	
		time ./integrate $n_threads 
	done
done
