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

# TEST FILES
## testcfiles
All of these files were used in the Interactive Mode test cases, explained below. 
### generateatest.c
Given an integer input, the program will return a string of the character 'a' repeated the integer input number of times. So for example, the input 3 would result in 'aaa'.
### sumtest.c
Given at least two integer inputs, the program will add them all together and return the sum as an integer.


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
    ./testcfiles/generateatest 10 | ./testcfiles/sumtest 5 15 > testoutput/test3output.txt
    cat testoutput/test3output.txt

Expected:
20
Result:
20

## Test Case 4 - Wildcard and Redirection Test
    ./testcfiles/generateatest 3 > testoutput/a1.txt
    ./testcfiles/generateatest 5 > testoutput/a2.txt
    ./testcfiles/generateatest 7 > testoutput/a3.txt
    cat testoutput/a*.txt | ./testcfiles/sumtest 10 20 > testoutput/test4output.txt
    cat testoutput/test4output.txt

Expected:
30
Result:
30

## Test Case 5 - Piping Test 02
    ./testcfiles/generateatest 100 | ./testcfiles/sumtest 50 50 > testoutput/test5output.txt
    cat testoutput/test5output.txt

Expected:
100
Result:
100

## Test Case 6 - Multi-Piping Test
    ./testcfiles/generateatest 5 | ./testcfiles/sumtest 10 | ./testcfiles/sumtest 5 > testoutput/test6output.txt
    cat testoutput/test6output.txt

Expected:
5
Result:
5

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



# BATCH MODE

## Test Case - echo and ls in batch mode

    $ cat myscript.sh
    echo hello
    ls testDir
    $ ./mysh myscript.sh
    hello
    subDir

- The cat command shows what the file "myscript.sh" contains. testDir contains just one subdirectory called "subDir". This test case showcases basic functionality in batch mode.


