#ifndef ATTENDANCE_DATA_H
#define ATTENDANCE_DATA_H

typedef struct {
    char dni[20];
    char type[10];      // "entrance" o "exit"
    char timestamp[32]; // "YYYY-MM-DD HH:MM:SS"
} AttendanceData;

#endif // ATTENDANCE_DATA_H