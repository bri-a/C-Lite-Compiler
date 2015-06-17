#!/bin/bash

# move to the root directory
# this script expects to be run from inside the testscripts directory
cd ..

echo "cleaning..."
echo
gmake clean
echo
echo "done cleaning"

echo "building... "
echo
gmake all
echo
echo "done building"

gmake run_SA1_test1

#make interpreter

cd ..
cd interpreter
gmake clean
gmake

./interpreter ../CS480_pcc_alco8653/quad.q
