#!/bin/bash
make all >> ./debug_out/make_out.txt
sudo insmod Hello_world >> ./debug_out/insmod_error.txt 2>&1 ./debug_out/insmod_sucess.txt
dmesg | tail -1 >> ./debug_out/output_dmesg.txt
lsmod | grep "Hello_World.ko" >> ./debug_out/module_check.txt
sudo rmmod Hello_world >> ./debug_out/rmmod_error.txt 2>&1 ./debug_out/rmmmod_sucess.txt
dmesg | tail -1 >> ./debug_out/output_dmesg.txt


