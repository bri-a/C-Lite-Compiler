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
./bin/lex_driver testcases/lex_tests/test1.lex > testcases/lex_tests/lex_test1.out
diff -Bw testcases/lex_tests/lex_test1.out testcases/lex_tests_expected/test1.lex.out
echo
echo "done Test Case 1"



echo "Test Case 2... "
echo
./bin/lex_driver testcases/lex_tests/test2.lex > testcases/lex_tests/lex_test2.out
diff -Bw testcases/lex_tests/lex_test2.out testcases/lex_tests_expected/test2.lex.out
echo
echo "done Test Case 2"



echo "Test Case 3... "
echo
./bin/lex_driver testcases/lex_tests/test3.lex > testcases/lex_tests/lex_test3.out
diff -Bw testcases/lex_tests/lex_test3.out testcases/lex_tests_expected/test3.lex.out
echo
echo "done Test Case 3"


echo "Test Case 4... "
echo
./bin/lex_driver testcases/lex_tests/test4.lex > testcases/lex_tests/lex_test4.out
diff -Bw testcases/lex_tests/lex_test4.out testcases/lex_tests_expected/test4.lex.out
echo
echo "done Test Case 4"



echo "Test Case 5... "
echo
./bin/lex_driver testcases/lex_tests/test5.lex > testcases/lex_tests/lex_test5.out
diff -Bw testcases/lex_tests/lex_test5.out testcases/lex_tests_expected/test5.lex.out
echo
echo "done Test Case 5"

echo "Test Case 6... "
echo
./bin/lex_driver testcases/lex_tests/DoNotRun_test6.lex > testcases/lex_tests/DoNotRun_test6.out
diff -Bw testcases/lex_tests/DoNotRun_test6.out testcases/lex_tests_expected/DoNotRun_test6.lex.out
echo
echo "done Test Case 6"

echo "Test Case 7... "
echo
./bin/lex_driver testcases/lex_tests/DoNotRun_test7.lex > testcases/lex_tests/DoNotRun_test7.out
diff -Bw testcases/lex_tests/DoNotRun_test7.out testcases/lex_tests_expected/DoNotRun_test7.lex.out
echo
echo "done Test Case 7"


echo "Test Case 8... "
echo
./bin/lex_driver testcases/lex_tests/test8.lex > testcases/lex_tests/lex_test8.out
diff -Bw testcases/lex_tests/lex_test8.out testcases/lex_tests_expected/test8.lex.out
echo
echo "done Test Case 8"



echo "Test Case 9... "
echo
./bin/lex_driver testcases/lex_tests/test9.lex > testcases/lex_tests/lex_test9.out
diff -Bw testcases/lex_tests/lex_test9.out testcases/lex_tests_expected/test9.lex.out
echo
echo "done Test Case 9"



echo "Test Case 10... "
echo
./bin/lex_driver testcases/lex_tests/test10.lex > testcases/lex_tests/lex_test10.out
diff -Bw testcases/lex_tests/lex_test10.out testcases/lex_tests_expected/test10.lex.out
echo
echo "done Test Case 10"


echo "Test Case 11... "
echo
./bin/lex_driver testcases/lex_tests/DoNotRun_test11.lex > testcases/lex_tests/DoNotRun_test11.lex.out
diff -Bw testcases/lex_tests/DoNotRun_test11.lex.out testcases/lex_tests_expected/DoNotRun_test11.lex.out
echo
echo "done Test Case 11"

rm -rf testcases/lex_tests/*.out

#done
