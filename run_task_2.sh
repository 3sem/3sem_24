#!/bin/bash

valgrind --leak-check=full --show-leak-kinds=all build/task_2/channel/channel $1 $2 $3
echo "> program finished with exit code $?"