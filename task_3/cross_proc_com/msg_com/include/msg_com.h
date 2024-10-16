#ifndef MSG_COM_H
#define MSG_COM_H

#define INPUT_FILENAME  "../data/input_file.txt"
#define OUTPUT_FILENAME "output_file.txt"

#define MSG_TYPE 1
#define BUFFER_CAPACITY LARGE_BUGGER_SIZE
#define MESSAGE_CAPACITY 8192

typedef struct _Massage {
    long mtype;
    char mtext[MESSAGE_CAPACITY];
} msg;

#endif