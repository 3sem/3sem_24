#include <stdio.h>

#include "data_exchange.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    perror("Error in number of command line arguments");
    return ERROR_NUMBER_ARGC;
  }

  data_exchange_file(argv + 1);

  return 0;
}
