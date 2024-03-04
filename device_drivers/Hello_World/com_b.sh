#!/bin/bash

rm ./debug_out/*
printf "\n\n\n\n\n"
make all | tee ./debug_out/make_out.txt
printf "\n\n\n\n\n"
echo `sudo insmod $1.ko` | tee ./debug_out/insmod_status.txt 2>&1
printf "\n\n\n\n\n"
dmesg | tail -10 | tee ./debug_out/output_dmesg.txt
printf "\n\n\n\n\n"
echo `lsmod | grep $1` | tee ./debug_out/module_check.txt
printf "\n\n\n\n\n"
echo `sudo rmmod $1.ko` | tee  ./debug_out/rmmod_status.txt 2>&1 
printf "\n\n\n\n\n"
dmesg | tail -10 | tee ./debug_out/output_dmesg.txt
printf "\n\n\n\n\n"
echo "Want to delete the .ko and remaining files ? [Y/n]"
read CLEAN_OUT
if [ "$CLEAN_OUT" == "Y" ] ; then
	printf "\n\n"
	echo `make clean` | tee ./debug_out/make_clean.txt
	printf "\n\n"
else
	echo "no cleaning is done"
fi

