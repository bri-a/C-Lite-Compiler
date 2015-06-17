
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
./bin/td_driver testcases/td_tests/test1.td > testcases/td_tests/td_test1.out
diff -Bw testcases/td_tests/td_test1.out testcases/td_tests_expected/test1.td.out
echo "done Test Case 1"
echo



#echo "Test Case 2... "
#echo
#./bin/td_driver testcases/td_tests/test2.td > testcases/td_tests/td_test2.out
#diff -Bw testcases/td_tests/td_test2.out testcases/td_tests_expected/test2.td.out
#echo
#echo "done Test Case 2"



#echo "Test Case 2_1... "
#echo
#./bin/td_driver testcases/td_tests/test2_1.td > testcases/td_tests/td_test2_1.out
#diff -Bw testcases/td_tests/td_test2_1.out testcases/td_tests_expected/test2_1.td.out
#echo
#echo "done Test Case 2_1"


#echo "Test Case 2_2... "
#echo
#./bin/td_driver testcases/td_tests/test2_2.td > testcases/td_tests/td_test2_2.out
#diff -Bw testcases/td_tests/td_test2_2.out testcases/td_tests_expected/test2_2.td.out
#echo
#echo "done Test Case 2_2"


echo
echo "Test Case 3... "
./bin/td_driver testcases/td_tests/test3.td > testcases/td_tests/td_test3.out
diff -Bw testcases/td_tests/td_test3.out testcases/td_tests_expected/test3.td.out
echo "done Test Case 3"
echo

echo
echo "Test Case 4... "
./bin/td_driver testcases/td_tests/test4.td > testcases/td_tests/td_test4.out
diff -Bw testcases/td_tests/td_test4.out testcases/td_tests_expected/test4.td.out
echo "done Test Case 4"
echo

echo
echo "Test Case 6... "
./bin/td_driver testcases/td_tests/test6.td > testcases/td_tests/td_test6.out
diff -Bw testcases/td_tests/td_test6.out testcases/td_tests_expected/test6.td.out
echo "done Test Case 6"
echo

echo "Test Case 7... "
echo
./bin/td_driver testcases/td_tests/test7.td > testcases/td_tests/td_test7.out
diff -Bw testcases/td_tests/td_test7.out testcases/td_tests_expected/test7.td.out
echo
echo "done Test Case 7"


echo "Test Case 8... "
echo
./bin/td_driver testcases/td_tests/test8.td > testcases/td_tests/td_test8.out
diff -Bw testcases/td_tests/td_test8.out testcases/td_tests_expected/test8.td.out
echo
echo "done Test Case 8"



#echo "Test Case 10a... "
#echo
#bin/td_driver testcases/td_tests/test10a.td > testcases/td_tests/td_test10a.out
#diff -Bw testcases/td_tests/td_test10a.out testcases/td_tests_expected/test10a.td.out
#echo
#echo "done Test Case 10a"

rm -rf testcases/td_tests/*.out

#done
