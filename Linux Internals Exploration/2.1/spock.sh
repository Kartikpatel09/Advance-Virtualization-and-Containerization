#!/bin/bash
make
insmod q2a_driver.ko
gcc q2a_user.c && ./a.out
make clean
rmmod q2a_driver.ko