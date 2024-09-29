# Terminal emulator
Computer technologies, task 3, file sharing speed comparison.

## Author
Усолцьев Иван Алексеевич, студент 2-го гурса ФРКТ МФТИ, группа Б01-307.

## Usage
To build the project go to [UsoltsevI/task3/](UsoltsevI/task3/) folder and run the following command:
```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
```

Then use it:
```
VERBOSE=1 cmake --build ./build
```

Usage example:
```
./build/filesharing <file_name>
./build/filesharing fourgb
```

*note:* You _have to_ use cmake to build the project if you don't want to dealing with hemorrhoids.
