#ifndef ATTENDANCE_QUEUE_H
#define ATTENDANCE_QUEUE_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "attendance_data.h"

#define QUEUE_LENGTH 10

void queue_init(void);
QueueHandle_t queue_get_handle_list(void);

#endif // ATTENDANCE_QUEUE_H