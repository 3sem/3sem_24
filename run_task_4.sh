#!/bin/bash

valgrind --leak-check=full --show-leak-kinds=all build/task_4/integral/integral
echo "> program finished with exit code $?"