#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"



# 
# AFTER RUNNING THIS SETUP, YOU CAN BIND PROCESSES TO SPECIFIC IP
#
#  Usage:
#  
#       LD_PRELOAD=/path/to/ipbind.so <command>
#	
#	    eg:
#	         	
#       you can specify the address to use at runtime like so:
#	    
#       LD_PRELOAD=/home/yourname/ipbind.so BIND_SRC=123.123.123.123 curl whatismyip.akamai.com 
#           
#       or
#
#       LD_PRELOAD=/home/yourname/ipbind.so BIND_SRC=123.123.123.123 bash    (and in that session do whatever you like)
#
#       ofcourse you need to change the ip-address to a real public ip assigned to your machine
#


# GCC LIB (change if needed)

GCC_FOLDER=/usr/lib/gcc/x86_64-linux-gnu/5.4.0


# MAKE STUFF WORK

cd ${GCC_FOLDER}
sudo cp crtbeginT.o crtbeginT.orig.o
sudo cp crtbeginS.o crtbeginT.o
cd ${DIR}
sudo gcc -fPIC -static --static -shared -o ipbind.so ipbind.c -lc -ldl


# RESTORE THE STUFF BACK TO ORIGINAL

cd ${GCC_FOLDER}
sudo cp crtbeginT.orig.o crtbeginT.o
sudo rm crtbeginT.orig.o
cd ${DIR}


exit 
