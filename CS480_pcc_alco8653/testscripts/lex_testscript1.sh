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
gmake run_lex_test1
echo
echo "done Test Case 1"

echo "Test Valgrind "
echo
gmake run_valgrind_lex
echo
echo "done Valgrind"

echo "diff test1..."
echo
gmake diff_lex_test1
echo
echo "done diff"

#done
