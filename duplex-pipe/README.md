# Duplex pipe

## Overview

The Duplex pipe is a upgraded pipe that allows bidirectional transfering of big data between processes.

## Examples

The files that are used to test the program are stored [here](source/receive_send.c) are global constans. Please use the `.bd` file extension for your big data files.

To create a file of size 4gb:
```
time dd if=/dev/urandom of=path/to/file bs=1048576 count=4096
```

Two make sure that the two files are identical run:

```
bash cmp2files.sh examples/send.bd examples/receive.bd
```

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
./main
```


