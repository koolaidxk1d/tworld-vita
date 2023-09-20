#!/bin/zsh

cmake -S .. -B . && make clean && make && cp tworld-vita.self eboot.bin && ftp -u ftp://192.168.0.122:1337/ux0:/app/TWKK00001/eboot.bin eboot.bin

