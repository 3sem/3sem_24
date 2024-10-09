#include "shared_mem.h"
#include "message_queue.h"
#include "fifo.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    computeTimeExchangeData(exchangeSharedMemory);
    computeTimeExchangeData(exchangeMessageQueue);
    computeTimeExchangeData(exchangeFifo);

    return 0;
}
