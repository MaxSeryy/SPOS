#ifndef SHELL_H
#define SHELL_H

#include "sync.h"

// shared between the shell task and any other task that wants to
// print to UART or push a log line (e.g. Task_Blinker)
extern os_mutex_t print_mutex;
extern os_queue_t log_queue;

void shell_init(void);   // init print_mutex + log_queue, call once from main() before os_start()
void Task_Terminal(void);

#endif // SHELL_H