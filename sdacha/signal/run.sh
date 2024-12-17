#!/bin/bash

make

echo "./signal 10 sender.txt receiver.txt"
./signal 10 sender.txt receiver.txt
