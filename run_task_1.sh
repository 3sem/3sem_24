#!/bin/bash

valgrind --leak-check=full --show-leak-kinds=all build/task_1/cmd_emulator/cmd_emulator
echo "> program finished with exit code $?"