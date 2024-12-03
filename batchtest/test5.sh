#!/bin/bash
./testcfiles/product 3 10 | ./testcfiles/product 2 > batchoutput/test5output.txt
cat batchoutput/test5output.txt
