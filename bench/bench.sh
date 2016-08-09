#!/bin/bash

#####################################################
# 
# [ bench.sh ]
# 
# do all benchs and print result to bench.csv
#
#
# [ usage ]
#
# bench.sh <msg_size> <repeat_count>
# 
# e.g) bench.sh 4096 50  ==> send 4k msg, 50 times
#
#####################################################

out_fname="bench.csv"

benchs[0]="fifo"
benchs[1]="mqueue"
benchs[2]="shm"
benchs[3]="socket"

# build
make

# print header
echo "ipc_type,msg_size,repeat_count,time" > ${out_fname}

# do all benchs
for idx in 0 1 2 3
do
	echo -n ${benchs[$idx]} >> ${out_fname}
	echo -n "," >> ${out_fname}

	echo -n "$1,$2," >> ${out_fname}

	cd ${benchs[$idx]}
	./server &
	sleep 1
	./client $1 $2 >> ../${out_fname}
	cd ../
done
