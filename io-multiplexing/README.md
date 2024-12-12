## Build

```
make
```

## Usage

Run the server in terminal 1: 

```
./server.x
```

Run the client in terminal 2:

```
./client.x
```

To register a transmit & receive fifo files type:

```
REGISTER a b
```

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
