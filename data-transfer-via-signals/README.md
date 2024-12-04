### Intro

Before you build the project please generate a large file:

```
dd if=/dev/urandom of=input.dat bs=1KB count=1
```
### Comparison

Let's compare the control of signal overflow using custom sleep & semaphores.

## Custom sleep

![](pictures/sleep.png)

# Discussion

1. Inefficient:

    Fixed sleep times may overcompensate, wasting time when the receiver is ready sooner than the sleep duration.
    The sender has no awareness of the receiver's actual state.

2. Inflexible:

    The sleep duration must be tuned manually, which depends on the signal-processing speed of the receiver. This tuning may not generalize across systems or workloads.

3. Risk of Overflow:

    If the receiver is slower than expected, signals may still overflow despite the delay.

# Results

![](pictures/sleep_time.png)

## Semaphores

![](pictures/sem.png)

# Discussion

1. Efficient Synchronization:

    The sender is blocked until the receiver signals readiness via the semaphore, preventing wasted CPU cycles.

2. Dynamic Behavior:

    Automatically adapts to the receiver's processing speed without manual tuning.

3. Overflow Prevention:

    Prevents signal overflow by ensuring that signals are only sent when the receiver can handle them.

4. Scalable:

    Works well for scenarios with multiple senders or receivers.

# Results

![](pictures/sem_time.png)

### Conclusion

| **Feature**                 | **Custom Sleep**                   | **Semaphore**                          |
|-----------------------------|------------------------------------|---------------------------------------|
| **Implementation Complexity** | Simple                            | Moderate                              |
| **CPU Efficiency**           | Inefficient (fixed delays)         | Efficient (blocks until ready)        |
| **Receiver Awareness**       | None                               | Aware (receiver signals readiness)    |
| **Risk of Overflow**         | Moderate                          | None                                  |
| **System Resource Usage**    | None                               | Semaphore (shared resource)           |
| **Scalability**              | Poor (tuning per workload/system)  | Good (dynamic readiness signaling)    |
| **Portability**              | High (works everywhere)            | High (POSIX-compliant systems)        |


Custom Sleep is quick and easy but lacks flexibility and can lead to inefficiencies or overflow.
Semaphores provide robust synchronization, ensuring efficient operation and preventing signal overflow but require additional setup and system resources.

### Build

```
make
```

### Run

```
make run 
```
