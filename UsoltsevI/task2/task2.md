# Full-Duplex Pipe
Computer technologies, task 2, Full-Duplex Pipe.

## Author
Усолцьев Иван Алексеевич, студент 2-го гурса ФРКТ МФТИ, группа Б01-307.

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

Please, make sure you have not a file with a name "temporary".

## Test results
In my laptop:
```
$ ./build/bigpipe 4294967295 < fourgb 
BigPipe sending started
file_size: 4294967295
fd = 3
SENDING IS FINISHED
RECIEVING IS FINISHED
child_recieved: 4294967296
SENDING IS FINISHED
child_sent: 4294967296
RECIEVING IS FINISHED
BigPipe sending finished
Bytes sent: 4294967296, bytes recieved: 4294967296
Executed time: 62.378918
```

And another one:
```
$ ./build/bigpipe 32768 < short 
BigPipe sending started
file_size: 32768
SENDING IS FINISHED
fd = 3
RECIEVING IS FINISHED
child_recieved: 32768
SENDING IS FINISHED
child_sent: 32768
RECIEVING IS FINISHED
BigPipe sending finished
Bytes sent: 32768, bytes recieved: 32768
Executed time: 0.001007
```