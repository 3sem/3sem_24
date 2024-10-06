#!/bin/bash

valgrind --leak-check=full --show-leak-kinds=all build/task_2/channel/channel
echo "> program finished with exit code $?"