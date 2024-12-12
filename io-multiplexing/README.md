## Build

```
make
```

Run the server in terminal 1: 

```
./server.x
```

Run the client in terminal 2:

```
./client.x
```

To register a transim & receive fifo type:

```
REGISTER a b
```

After that the program will close.

To simply receive a file:

```
echo "GET test.txt" > a
```

And you will see the test.txt written in b:

```
cat b
```

To close the server while running the client binary:

```
CLOSE 
```
