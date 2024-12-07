#pragma once

enum error_code {
  ERROR_NUMBER_ARGC = 1,
  ERROR_CREATE_PIPE = 2,
  ERROR_CALL_FORK   = 3,
};

int data_exchange_file(char **fnames);
