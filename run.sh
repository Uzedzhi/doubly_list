#!/bin/bash
LINUX_FLAGS="-O3 -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr"
g++ -L/mnt/c/Users/Azerty/my_project/my_libs list.cpp -Wl,-rpath=/mnt/c/Users/Azerty/my_project/my_libs/ $LINUX_FLAGS -o list   -I/mnt/c/Users/Azerty/my_project/my_libs/   -lerror_manage -lbetter_output
