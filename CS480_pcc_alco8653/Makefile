#############################################################################
# File name:  Makefile
# Author:     Brianna Alcoran
# Date:       2.2.15
# Class:      CS480
# Assignment: Compilers
# Purpose: 	  This is the Makefile for the compiler 
#############################################################################

TARGETS 			= bin/st_driver bin/lex_driver bin/td_driver bin/bu_driver bin/pcc
CC 						= gcc
CFLAGS 				= -Wall -g
OBJECTS				= bin/er.o bin/list.o
ALL_OBJECTS		= bin/er.o bin/list.o bin/st.o bin/ct.o bin/rts.o bin/lex.o \
								bin/td.o bin/bu.o bin/qd.o
ST_OBJECTS 		= bin/st_driver.o bin/st.o ${OBJECTS}
LEX_OBJECTS 	= bin/lex_driver.o bin/lex.o bin/st.o ${OBJECTS} bin/ct.o \
								bin/rts.o
TD_OBJECTS 		= bin/td_driver.o bin/td.o ${OBJECTS} bin/lex.o bin/st.o \
								bin/rts.o bin/ct.o bin/qd.o bin/bu.o
BU_OBJECTS 		= bin/bu_driver.o bin/bu.o bin/td.o ${OBJECTS} bin/lex.o bin/st.o \
								bin/rts.o bin/ct.o bin/qd.o
PCC_OBJECTS		= bin/pcc.o ${ALL_OBJECTS}
DEPS					= src/include/list.h src/include/pcc13defines.h
ST_DEPS 			= src/include/er.h src/include/st.h ${DEPS}
LEX_DEPS 			= ${ST_DEPS} src/include/lex.h src/include/ct.h src/include/rts.h
ALL_DEPS			= ${LEX_DEPS} src/include/td.h src/include/bu.h src/include/qd.h

all: ${TARGETS}

#		SYMBOL TABLE		#
bin/st_driver: ${ST_OBJECTS}
	${CC} ${CFLAGS} -o bin/st_driver ${ST_OBJECTS}
	
bin/st_driver.o: src/drivers/st_driver.c ${ST_DEPS}
	${CC} ${CFLAGS} -o bin/st_driver.o -c src/drivers/st_driver.c
	
bin/st.o: src/st/st.c ${ST_DEPS}
	${CC} ${CFLAGS} -c src/st/st.c -o bin/st.o 

#		ERROR HANDLER		#
bin/er.o: src/er/er.c src/include/er.h ${DEPS}
	${CC} ${CFLAGS} -c src/er/er.c -o bin/er.o

#		LIST		#
bin/list.o: src/util/list.c ${DEPS}
	${CC} ${CFLAGS} -c src/util/list.c -o bin/list.o

#		LEXER		#
bin/lex_driver: ${LEX_OBJECTS}
	${CC} ${CFLAGS} -o bin/lex_driver ${LEX_OBJECTS}

bin/lex_driver.o: src/drivers/lex_driver.c ${LEX_DEPS}
	${CC} ${CFLAGS} -o bin/lex_driver.o -c src/drivers/lex_driver.c

bin/lex.o: src/lex/lex.c ${LEX_DEPS}
	${CC} ${CFLAGS} -c src/lex/lex.c -o bin/lex.o

#		CONSTANT TABLE		#
bin/ct.o: src/ct/ct.c src/include/ct.h ${DEPS}
	${CC} ${CFLAGS} -c src/ct/ct.c -o bin/ct.o

#		RUNTIME STACK		#
bin/rts.o: src/rts/rts.c src/include/rts.h ${DEPS}
	${CC} ${CFLAGS} -c src/rts/rts.c -o bin/rts.o

#		TD PARSER			#
bin/td_driver: ${TD_OBJECTS}
	${CC} ${CFLAGS} -o bin/td_driver ${TD_OBJECTS}
	
bin/td_driver.o: src/drivers/td_driver.c src/include/td.h src/include/bu.h ${LEX_DEPS}
	${CC} ${CFLAGS} -o bin/td_driver.o -c src/drivers/td_driver.c

bin/td.o: src/td/td.c src/include/td.h src/include/qd.h src/include/bu.h
	${CC} ${CFLAGS} -c src/td/td.c -o bin/td.o


#		BU PARSER			#
bin/bu_driver: ${BU_OBJECTS}
	${CC} ${CFLAGS} -o bin/bu_driver ${BU_OBJECTS}
	
bin/bu_driver.o: src/drivers/bu_driver.c src/include/bu.h ${LEX_DEPS}
	${CC} ${CFLAGS} -o bin/bu_driver.o -c src/drivers/bu_driver.c

bin/bu.o: src/bu/bu.c src/include/bu.h
	${CC} ${CFLAGS} -c src/bu/bu.c -o bin/bu.o

#  sQUAAAAAD		#
bin/qd.o: src/quad/qd.c src/include/qd.h ${DEPS}
	${CC} ${CFLAGS} -c src/quad/qd.c -o bin/qd.o

#		PCC!!!!!		#
bin/pcc: ${PCC_OBJECTS}
	${CC} ${CFLAGS} -o bin/pcc ${PCC_OBJECTS}
	
bin/pcc.o: src/drivers/pcc.c ${ALL_DEPS}
	${CC} ${CFLAGS} -o bin/pcc.o -c src/drivers/pcc.c


#		CLEAN		#
clean:
	rm -rf bin/*.o *.q *.out

#		ST VALGRIND & TEST	#
run_valgrind_st: bin/st_driver
	valgrind -v --leak-check=yes ./bin/st_driver testcases/st_tests/st_testcase1
	
run_st_test1: bin/st_driver
	./bin/st_driver testcases/st_tests/st_testcase1

#		LEX VALGRIND & TEST		#
run_valgrind_lex: bin/lex_driver
	valgrind -v --leak-check=yes ./bin/lex_driver testcases/lex_tests/lex_test1
	
run_lex_test1: bin/lex_driver
	./bin/lex_driver testcases/lex_tests/lex_test1
	
diff_lex_test1: bin/lex_driver
	./bin/lex_driver testcases/lex_tests/lex_test1 > testcases/lex_tests/lex_test1.out
	diff -Bw testcases/lex_tests/lex_test1.out \
	testcases/lex_tests_expected/test1.lex.out
	
#		TD VALGRIND & TEST		#
run_td_valgrind_test1: bin/td_driver
	valgrind -v --leak-check=yes ./bin/td_driver testcases/td_tests/td_test1

run_td_test1: bin/td_driver
	./bin/td_driver testcases/td_tests/td_test1

diff_td_test1: bin/td_driver
	./bin/td_driver testcases/td_tests/td_test1 > testcases/td_tests/td_test1.out
	diff -Bw testcases/td_tests/td_test1.out \
	testcases/td_tests_expected/td_test1.out.expected

diff_td_test4: bin/td_driver
	./bin/td_driver testcases/td_tests/td_test4 > testcases/td_tests/td_test4.out
	diff -Bw testcases/td_tests/td_test4.out \
	testcases/td_tests_expected/td_test4.out.expected
	
	
#		BU PARSER			#
run_bu_valgrind_test1: bin/bu_driver
	valgrind -v --leak-check=yes ./bin/bu_driver testcases/bu_tests/bu_test1
	
run_bu_test1: bin/bu_driver
	./bin/bu_driver testcases/bu_tests/bu_test1

	
#		SA PART 1		#
run_SA1_valgrind_test1: bin/pcc
	valgrind -v --leak-check=yes ./bin/pcc testcases/sa_tests/sa_testcase1

run_SA1_test1: bin/pcc
	./bin/pcc testcases/sa_tests/sa_testcase1
	
	#		SA PART 2		#
run_sa2_valgrind_test1: bin/pcc
	valgrind -v --leak-check=yes ./bin/pcc testcases/sa2_tests/sa2_test1

run_sa2_test1: bin/pcc
	./bin/pcc testcases/sa2_tests/sa2_test1


	#		SA PART 3		#
run_test_SA3_1: bin/pcc bin/inter
	script run_test_SA3_1.full.out -c './test_SA3_full.sh 1'
	
run_test_SA3_2: bin/pcc bin/inter
	script run_test_SA3_2.full.out -c './test_SA3_full.sh 2'
	
run_test_SA3_3: bin/pcc bin/inter
	script run_test_SA3_3.full.out -c './test_SA3_full.sh 3'
	
run_test_SA3_4: bin/pcc bin/inter
	script run_test_SA3_4.full.out -c './test_SA3_full.sh 4'
	
run_test_SA3_5: bin/pcc bin/inter
	script run_test_SA3_5.full.out -c './test_SA3_full.sh 5'
	
run_test_SA3_6: bin/pcc bin/inter
	script run_test_SA3_6.full.out -c './test_SA3_full.sh 6'
	
run_test_SA3_7: bin/pcc bin/inter
	script run_test_SA3_7.full.out -c 'echo $(USER); \
	bin/pcc test_SA3_7.c --sa3typecheck; echo $(USER)'
	
run_test_SA3_8: bin/pcc bin/inter
	script run_test_SA3_8.full.out -c './test_SA3_full.sh 8'
	
	#		FINAL INTEGRATION		#
run_test_FI_1: bin/pcc bin/inter
	script run_test_FI_1.full.out -c './test_FI_full.sh 1'
	
run_test_FI_2: bin/pcc bin/inter
	script run_test_FI_2.full.out -c './test_FI_full.sh 2'
	
run_test_FI_3: bin/pcc bin/inter
	script run_test_FI_3.full.out -c './test_FI_full.sh 3'
	
run_test_FI_4: bin/pcc bin/inter
	script run_test_FI_4.full.out -c './test_FI_full.sh 4'
	
run_test_FI_5: bin/pcc bin/inter
	script run_test_FI_5.full.out -c './test_FI_full.sh 5'
	
run_test_FI_6: bin/pcc bin/inter
	script run_test_FI_6.full.out -c './test_FI_full.sh 6'
	
run_test_FI_7: bin/pcc bin/inter
	script run_test_FI_7.full.out -c './test_FI_full.sh 7'
	
run_test_FI_8: bin/pcc bin/inter
	script run_test_FI_8.full.out -c './test_FI_full.sh 8'

