#!/bin/bash

#valgrind --leak-check=full --show-leak-kinds=all 
build/task_6/processmon/processmon $1 $2
echo "> program finished with exit code $?"