# Project 3 Description & Test Plan

Project 3 Systems Programming CS214 w/ Menendez Authors: Ayushi Joshi (aj934) and Sriya Vemuri (sv660)

Description: The My Shell (mysh) project implements a functional command-line shell in C. The shell is capable of reading user input in interactive or batch mode, tokenizing it into commands and arguments, handling redirections and pipes, processing wildcard expansions, forking child processes and opening files and reassigning stdin/stdout as needed. 

# My Shell Design

In this project, we have made use of various helper functions as well as Prof Menendez's implementation of arraylists from lecture. The main function uses isatty() and argc to determine whether the program is running in interactive or batch mode. 

Function for Reading Input:
readAndGetLine: Reads input line by line from a file descriptor using a buffer, dynamically allocating memory as needed.

Function for Parsing Input:
tokenize: Splits input into tokens (e.g., commands, arguments, symbols like <, >, |), supports wildcard expansion using glob, and stores results in an arraylist for processing.

Function for Command Execution:
is_builtin and execute_builtin: Identifies and handles built-in commands like cd, pwd, which, and exit.
execute_command: Spawns child processes to execute external commands, managing pipes, input/output redirection, and error handling.

Function for Pipelines:
input_to_command_execution: Orchestrates execution of commands connected by pipes, setting up the correct file descriptors for each command.

Function to resolve path for execv:
resolve_command_path

# TEST FILES
## testcfiles
All of these files were used in the Interactive Mode test cases, explained below. 
### generateatest.c
Given an integer input, the program will return a string of the character 'a' repeated the integer input number of times. So for example, the input 3 would result in 'aaa'.
### sumtest.c
Given at least two integer inputs, the program will add them all together and return the sum as an integer.
### product.c
Given at least two integer inputs, the program will multiply them all together and return the product as an integer.
### file1, file2, and file3.txt
All three of these text files have different integers.

## batchtest
All of these files are Interactive Mode test cases rewritten so that it can be tested in batch mode (see below).

## testDir/subDir
Directory with subdirectory subDir that consists of three files.
### file1, file2, and file3.txt
All three of these text files have the line "This is <filename>!" where filename is replaced with file1, file2, or file3 respectively.

## signal_test.c
Used to test error messaging for signals (specifically the exact output for error messaging).

# TEST OUTPUT FILES
## testoutput
All of the files in here are outputs for interactive mode tests
## batchoutput
All of the files in here are outputs for batch mode tests


# INTERACTIVE MODE TEST CASES

The following test cases utilize the simple C files written in the testcfiles directory. Each test case's result is outputted to their respective .txt file in the testoutput folder.
The expected result and the actual result in the terminal is below. *Please note that all of the results were preceeded with mysh> and additionally, the shell upon startup said "Welcome to my shell!" Upon exitting, it said, "mysh: exiting".*

## Test Case 1 - Redirection Test 01
    `./testcfiles/generateatest 10 > testoutput/test1output.txt
    cat testoutput/test1output.txt`

Expected:
`aaaaaaaaaa`
Result:
`aaaaaaaaaa`

## Test Case 2 - Redirection Test 02
    `./testcfiles/sumtest 5 15 20 > testoutput/test2output.txt
    cat testoutput/test2output.txt`

Expected:
`40`
Result:
`40`

## Test Case 3 - Piping Test 01
`./testcfiles/sumtest 3 5 | ./testcfiles/product 2 > testoutput/test3output.txt
cat testoutput/test3output.txt`

Expected:
`16`
Result:
`16`

## Test Case 4 - Wildcard Test
`cat testcfiles/*.txt > testoutput/test4output.txt
cat testoutput/test4output.txt`

Expected:
`2 4 6
3 5
7`
Result:
`2 4 6
3 5
7`

## Test Case 5 - Piping Test 02
`./testcfiles/product 3 10 | ./testcfiles/product 2 > testoutput/test5output.txt
cat testoutput/test5output.txt`

Expected:
`60`
Result:
`60`

## Test Case 6 - Multi-Piping Test
`./testcfiles/product 7 13 | ./testcfiles/product 2 | ./testcfiles/product 2 > testoutput/test6output.txt
cat testoutput/test6output.txt`

Expected:
`364`
Result:
`364`

## Test Case 7 - Error Handling for Nonexistent Files
    `cat testoutput/nonexistent.txt | ./testcfiles/sumtest 10 20 > testoutput/test7output.txt`

Expected:
`cat: testoutput/nonexistent.txt: No such file or directory
mysh: Command failed with code 1`

Result:
`cat: testoutput/nonexistent.txt: No such file or directory
mysh: Command failed with code 1`

## Test Case 8 - Error Handling for Incorrect Input + Redirection
    `./testcfiles/sumtest "invalid" > testoutput/test8output.txt
    cat testoutput/test8output.txt`

Expected:
`0`
Result:
`0`

## Test Case 9 - Built In Commands

    Welcome to my shell!
    mysh> pwd
    /common/home/aj934/Documents/P3
    mysh> cd 
    Failure with cd: Missing arguments.
    mysh> cd testDir
    mysh> pwd
    /common/home/aj934/Documents/P3/testDir
    mysh> cd ../..
    mysh> pwd
    /common/home/aj934/Documents
    mysh> cd ..
    mysh> pwd
    /common/home/aj934
    mysh> cd sdjfn
    cd: No such file or directory
    mysh> which ls
    /usr/bin/ls
    mysh> exit
    mysh: exiting

Exit with arguments:

    Welcome to my shell!
    mysh> exit exiting now
    exiting now mysh: exiting


- This test case shows the working of built in commands: which, pwd, cd, and exit. As shown by the outputs, they are all working as expected.

  
## Test Case 10 - Wildcards

    Welcome to my shell!
    mysh> cd testDir
    mysh> cd subDir
    mysh> ls *.txt
    file1.txt  file2.txt  file3.txt
    mysh> cat *.txt
    this is file1!
    this is file2!
    this is file3!

- This test incorporated the use of wildcard(*) with ls and cat. Both commands work as expected.

## Test Case 11 - Non-zero status code

    Welcome to my shell!
    mysh> ls sdkjfn
    ls: cannot access 'sdkjfn': No such file or directory
    mysh: Command failed with code 1

## Test Case 12 - Error testing: Signals

    mysh> ./signal_test
    My PID is: 2433750
    Terminated by signal: Interrupt

- In this test, we have created a file names signal_test that runs for a long time. We terminate this process in the middle by entering kill -SIGINT <PID> in another terminal window. The PID is given to us by signal_test. As expected, mysh prints Terminated by signal: Interrupt using psignal. 

# BATCH MODE TEST CASES
The following test cases utilize the shell in bash mode. Test cases 1 - 8 are the same as the respective interactive mode test cases, except it is written in a .sh file. For the batch mode test, the .sh file is executed in the format ./mysh < batchtest/test<n>.sh, where n is the test case number. The output is saved in a text file in batchoutput, but also printed to the shell. The result printed here is the result printed in the shell.

## Test Case 1 - Redirection Test 01
`./mysh < batchtest/test1.sh`

Expected:
`aaaaaaaaaa`
Result:
`aaaaaaaaaa`

## Test Case 2 - Redirection Test 02
`./mysh < batchtest/test2.sh`

Expected:
`40`
Result:
`40`

## Test Case 3 - Piping Test 01
`./mysh < batchtest/test3.sh`

Expected:
`16`
Result:
`16`

## Test Case 4 - Wildcard Test
`./mysh < batchtest/test4.sh`

Expected:
`2 4 6
3 5
7`
Result:
`2 4 6
3 5
7`

## Test Case 5 - Piping Test 02
`./mysh < batchtest/test5.sh`

Expected:
`60`
Result:
`60`

## Test Case 6 - Multi-Piping Test
`./mysh < batchtest/test6.sh`

Expected:
`364`
Result:
`364`

## Test Case 7 - Error Handling for Nonexistent Files
`./mysh < batchtest/test7.sh`

Expected:
`cat: batchoutput/nonexistent.txt: No such file or directory
mysh: Command failed with code 1`

Result:
`cat: batchoutput/nonexistent.txt: No such file or directory
mysh: Command failed with code 1`

## Test Case 8 - Error Handling for Incorrect Input + Redirection
`./mysh < batchtest/test8.sh`

Expected:
`0`
Result:
`0`

## Test Case 9  - echo and ls in batch mode
`$ cat myscript.sh
echo hello
ls testDir
$ ./mysh myscript.sh
hello
subDir`

The cat command shows what the file "myscript.sh" contains. testDir contains just one subdirectory called "subDir". This test case showcases basic functionality in batch mode.

## Test Case 10 - testing with no specified file (piping)

    `$ cat myscript.sh | ./mysh
    hello
    subDir`

# COMBINED INTERACTIVE AND BATCH MODE
We decided to truly test our implementation of mysh by having an interactive mode open an interactive mode that ran mysh in bash. Below is the result, which is what was expected.

`$ ./mysh
Welcome to my shell!
mysh> ./mysh
Welcome to my shell!
mysh> ./mysh myscript.sh
hello
subDir
mysh> exit
mysh: exiting
mysh> exit
mysh: exiting
$ `