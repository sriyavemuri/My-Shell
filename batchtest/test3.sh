#!/bin/bash
./testcfiles/sumtest 3 5 | ./testcfiles/product 2 > batchoutput/test3output.txt
cat batchoutput/test3output.txt
