

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
gmake
echo
echo "done building"

echo "Test Case 1... "
echo
bin/st_driver
echo
echo "done Test Case 1"

echo "cleaning..."
echo
gmake clean
echo
echo "done cleaning"

echo "Test Valgrind "
echo
gmake run_valgrind_st
echo
echo "done Valgrind"
