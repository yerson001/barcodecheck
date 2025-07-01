#include "attendance_queue.h"
#include "attendance_data.h"
static QueueHandle_t attendance_queue = NULL;

void queue_init(void) {
    if (attendance_queue == NULL) {
        attendance_queue = xQueueCreate(QUEUE_LENGTH, sizeof(AttendanceData));
    }
}

QueueHandle_t queue_get_handle_list(void) {
    return attendance_queue;
}