# Project 3 Description & Test Plan

Project 3 Systems Programming CS214 w/ Menendez Authors: Ayushi Joshi (aj934) and Sriya Vemuri (sv660)

Description: The My Shell (mysh) project implements a functional command-line shell in C. The shell is capable of reading user input in interactive or batch mode, tokenizing it into commands and arguments, handling redirections and pipes, processing wildcard expansions, forking child processes and opening files and reassigning stdin/stdout as needed. 


# TEST FILES
## testcfiles
All of these files were used in the Interactive Mode test cases, explained below. 
### generateatest.c
Given an integer input, the program will return a string of the character 'a' repeated the integer input number of times. So for example, the input 3 would result in 'aaa'.
### sumtest.c
Given at least two integer inputs, the program will add them all together and return the sum as an integer.
### product.c
Given at least two integer inputs, the program will multiply them all together and return the product as an integer.


# INTERACTIVE MODE

The following test cases utilize the simple C files written in the testcfiles directory. Each test case's result is outputted to their respective .txt file in the testoutput folder.
The expected result and the actual result in the terminal is below. Please note that all of the results were preceeded with mysh> and additionally, the shell upon startup said "Welcome to my shell!" Upon exitting, it said, "mysh: exiting". 

## Test Case 1 - Redirection Test 01
./testcfiles/generateatest 10 > testoutput/test1output.txt
cat testoutput/test1output.txt

Expected:
aaaaaaaaaa
Result:
aaaaaaaaaa

## Test Case 2 - Redirection Test 02
./testcfiles/sumtest 5 15 20 > testoutput/test2output.txt
cat testoutput/test2output.txt

Expected:
40
Result:
40

## Test Case 3 - Piping Test 01
./testcfiles/sumtest 3 5 | ./testcfiles/product 2 > testoutput/test3output.txt
cat testoutput/test3output.txt

Expected:
16
Result:
16

## Test Case 4 - Wildcard Test
cat testcfiles/*.txt > testoutput/test4output.txt
cat testoutput/test4output.txt

Expected:
2 4 6
3 5
7
Result:
2 4 6
3 5
7

## Test Case 5 - Piping Test 02
./testcfiles/product 3 10 | ./testcfiles/product 2 > testoutput/test5output.txt
cat testoutput/test5output.txt

Expected:
60
Result:
60

## Test Case 6 - Multi-Piping Test
./testcfiles/product 7 13 | ./testcfiles/product 2 | ./testcfiles/product 2 > testoutput/test6output.txt
cat testoutput/test6output.txt

Expected:
364
Result:
364

## Test Case 7 - Error Handling for Nonexistent Files
cat testoutput/nonexistent.txt | ./testcfiles/sumtest 10 20 > testoutput/test7output.txt

Expected:
No such file or directory error

Result:
cat: testoutput/nonexistent.txt: No such file or directory

## Test Case 8 - Error Handling for Incorrect Input + Redirection
./testcfiles/sumtest "invalid" > testoutput/test8output.txt
cat testoutput/test8output.txt

Expected:
0
Result:
0

# BATCH MODE
