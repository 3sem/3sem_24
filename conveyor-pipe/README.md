# Pipeline conveyor

## Overview

The Pipeline Conveyor Program is designed to simulate a conveyor belt system for managing and processing commands in a structured and efficient manner. This program accepts command files, processes them in accordance with predefined actions, and utilizes a pipeline concept to manage the flow of data and operations.

## Examples

The commands that are used to test the program are stored [here](examples/commands.txt).


## Build

```
make dirs
```
If you want to build debug version (with log output) then type:

```
make
```
If you want to build release version then type:

```
make release
```
Run:

```
./main examples/commands.txt
```

## System specs

**Compiler**: Debian clang version 16.0.6

**OS**: Kali GNU/Linux Rolling x86_64

**CPU**: AMD Ryzen 7 5800HS with Radeon Graphics, 3201 Mhz, 8 Core(s), 16 Logical Processor(s)

**RAM**: 16.0 GB

