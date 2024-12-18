#!/bin/bash

#valgrind --leak-check=full --show-leak-kinds=all 
build/task_4/integral/integral
echo "> integral calculation finished with exit code $?"

echo "> processing result:"
#valgrind --leak-check=full --show-leak-kinds=all 
build/task_4/results/res_process
echo "> result processing finished with exit code $?"