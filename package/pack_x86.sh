#!/bin/bash

exe="StartupMain"
#mkdir -p lib
des=$(pwd)
deplist=$(ldd $exe |awk '{if (match($3,"/")){printf("%s "),$3}}')
cp $deplist ${des}