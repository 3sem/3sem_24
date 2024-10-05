# Terminal emulator
Computer technologies, task 4, parallel computation by the Monte Carlo algorithm.

## Author
Усолцьев Иван Алексеевич, студент 2-го гурса ФРКТ МФТИ, группа Б01-307.

## Usage
To build the project go to [UsoltsevI/task4/](UsoltsevI/task4/) folder and run the following command:
```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
VERBOSE=1 cmake --build ./build
```

Usage example:
```
./build/filesharing <num_threads> <num_points> <math_munc>
./build/filesharing 4 10000 cos
```

## Test results
