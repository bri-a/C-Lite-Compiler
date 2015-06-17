#!/bin/bash
echo $USER
rm quad.q
bin/pcc test_FI_$1.c
echo
echo
echo "TEST FI $1"
echo
echo "Start Interpreter"
echo "================="
echo
bin/inter quad.q
echo $USER
