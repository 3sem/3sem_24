# Full-Duplex Pipe
Computer technologies, task 2, Full-Duplex Pipe.

## Author
Усолцьев Иван Алексеевич, студент 2-го гурса ФРКТ МФТИ, группа Б01-307.

## Brief description
This program sends a large file from parent to child and back. Implemented on two pipes, transmits data in 1 kilobyte chunks. 

The data in the child is saved int he temporary file named "temporary". After recieving by the parent the data is output to a temporary file named "out" sa an not to clog the terminal. Then both temporary files are removed.

For the program to work correctly, inform it about the file size when called, as shown in the example below.

## Usage
To build the project go to [UsoltsevI/task2/](UsoltsevI/task2/) folder and run the following command:
```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
```

Then use it:
```
VERBOSE=1 cmake --build ./build
```

Finally, to send file to child and back, tap:
```
./build/bigpipe <file_size_in_bytes> < <input_file_name>
```

As an example:
```
./build/bigpipe 4294967295 < fourgb 
```


To make a big file with random data use:
```
time dd if=/dev/urandom of=<output_file_name> bs=1048576 count=4096
```

An example:
```
time dd if=/dev/urandom of=fourgb bs=1048576 count=4096
```

Please, make sure you have not a file named "temporary" and a file named "out".
These are temporary files, that the programm removes after use.

## Test results
On my laptop:
```
$ ./build/bigpipe 4294967296 < fourgb
--BigPipe sending started
----------------file_size: 4294967296
----Recieved by the child: 4294967296
--------Sent by the child: 4294967296
-BigPipe sending finished
---------------Bytes sent: 4294967296
-----------Bytes recieved: 4294967296
------------Executed time: 64.013625 sec
```

And another one:
```
$ ./build/bigpipe 32768 < short 
--BigPipe sending started
----------------file_size: 32768
----Recieved by the child: 32768
--------Sent by the child: 32768
-BigPipe sending finished
---------------Bytes sent: 32768
-----------Bytes recieved: 32768
------------Executed time: 0.001341 sec
```