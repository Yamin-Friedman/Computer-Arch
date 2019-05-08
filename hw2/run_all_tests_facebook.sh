#!/bin/bash

echo Running tests...
echo The test succeed if there are no diffs printed.
echo

for filename in tests/test*.command; do
    test_num=`echo $filename | cut -d'.' -f1`
    echo $test_num
#    cat ${filename}
    bash ${filename} > ${test_num}.YoursOut
    diff_result=$(diff ${test_num}.out ${test_num}.YoursOut)
    if [ "$diff_result" != "" ]; then
        echo The test ${test_num} didnt pass
	diff ${test_num}.out ${test_num}.YoursOut
    fi
done

#for filename in tests/test*.out; do
#    test_num=`echo $filename | cut -d'.' -f1`
#    diff_result=$(diff ${test_num}.out ${test_num}.YoursOut)
#    if [ "$diff_result" != "" ]; then
#        echo The test ${test_num} didnt pass
#    fi
#done

echo
echo Ran all tests.
