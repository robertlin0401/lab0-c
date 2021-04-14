make clean
make
gcc -o queue.o -O1 -g -Wall -Werror -Idudect -I. -c -MMD -MF .queue.o.d queue_recursive_merge.c
gcc -o qtest qtest.o report.o console.o harness.o queue.o random.o dudect/constant.o dudect/fixture.o dudect/ttest.o linenoise.o -lm

