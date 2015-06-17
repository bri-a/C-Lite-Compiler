
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
./bin/pcc testcases/sa1_tests/test1.sa1 > testcases/sa1_tests/test1.sa1.my.out
diff -Bw testcases/sa1_tests/test1.sa1.my.out testcases/sa1_tests_expected/test1.sa1.out
echo
echo "done Test Case 1"



#echo "Test Case 2... "
#echo
#./bin/pcc testcases/sa1_tests/test2.sa1 > testcases/sa1_tests/test2.sa1.my.out
#diff -Bw testcases/sa1_tests/test2.sa1.my.out testcases/sa1_tests_expected/test2.sa1.out
#echo
#echo "done Test Case 2"


echo "Test Case 3... "
echo
./bin/pcc testcases/sa1_tests/test3.sa1 > testcases/sa1_tests/test3.sa1.my.out
diff -Bw testcases/sa1_tests/test3.sa1.my.out testcases/sa1_tests_expected/test3.sa1.out
echo
echo "done Test Case 3"

echo "Test Case 4... "
echo
./bin/pcc testcases/sa1_tests/test4.sa1 > testcases/sa1_tests/test4.sa1.my.out
diff -Bw testcases/sa1_tests/test4.sa1.my.out testcases/sa1_tests_expected/test4.sa1.out
echo
echo "done Test Case 4"

echo "Test Case 5... "
echo
./bin/pcc testcases/sa1_tests/test5.sa1 > testcases/sa1_tests/test5.sa1.my.out
diff -Bw testcases/sa1_tests/test5.sa1.my.out testcases/sa1_tests_expected/test5.sa1.out
echo
echo "done Test Case 5"


echo "Test Case 6... "
echo
./bin/pcc testcases/sa1_tests/test6.sa1 > testcases/sa1_tests/test6.sa1.my.out
diff -Bw testcases/sa1_tests/test6.sa1.my.out testcases/sa1_tests_expected/test6.sa1.out
echo
echo "done Test Case 6"

echo "Test Case 7... "
echo
./bin/pcc testcases/sa1_tests/test7.sa1 > testcases/sa1_tests/test7.sa1.my.out
diff -Bw testcases/sa1_tests/test7.sa1.my.out testcases/sa1_tests_expected/test7.sa1.out
echo
echo "done Test Case 7"


echo "Test Case 8... "
echo
./bin/pcc testcases/sa1_tests/test8.sa1 > testcases/sa1_tests/test8.sa1.my.out
diff -Bw testcases/sa1_tests/test8.sa1.my.out testcases/sa1_tests_expected/test8.sa1.out
echo
echo "done Test Case 8"


echo "Test Case 9... "
echo
./bin/pcc testcases/sa1_tests/test9.sa1 > testcases/sa1_tests/test9.sa1.my.out
diff -Bw testcases/sa1_tests/test9.sa1.my.out testcases/sa1_tests_expected/test9.sa1.out
echo
echo "done Test Case 9"


echo "Test Case 10... "
echo
./bin/pcc testcases/sa1_tests/test10.sa1 > testcases/sa1_tests/test10.sa1.my.out
diff -Bw testcases/sa1_tests/test10.sa1.my.out testcases/sa1_tests_expected/test10.sa1.out
echo
echo "done Test Case 10"

rm -rf testcases/sa1_tests/*.out

#done
