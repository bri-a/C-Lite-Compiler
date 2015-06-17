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

echo "Test Case 1... "
echo
gmake diff_td_test1
echo
echo "done Test Case 1"

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

echo "Test Case 4... "
echo
gmake diff_td_test4
echo
echo "done Test Case 4"


#done
