#include "logger.h"
#include <stdarg.h>
#include <time.h>
#include "esp_timer.h"

void logger_log(const char* level, const char* color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // Obtener timestamp en milisegundos
    int64_t time_us = esp_timer_get_time();
    printf("%s[%lld ms] [%s] ", color, time_us / 1000, level);

    vprintf(fmt, args);
    printf("%s\n", ANSI_COLOR_RESET);

    va_end(args);
}
