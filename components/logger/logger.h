#pragma once

#include <stdio.h>

// Colores ANSI personalizados (sin usar verde para evitar conflictos)
#define ANSI_COLOR_RESET     "\x1b[0m"
#define ANSI_COLOR_RED       "\x1b[31m"
#define ANSI_COLOR_YELLOW    "\x1b[33m"
#define ANSI_COLOR_BLUE      "\x1b[34m"
#define ANSI_COLOR_MAGENTA   "\x1b[35m"
#define ANSI_COLOR_CYAN      "\x1b[36m"
#define ANSI_COLOR_WHITE     "\x1b[97m"

// Log de información simple
#define LOG_INFO(msg) \
    printf(ANSI_COLOR_BLUE "[INFO] %s" ANSI_COLOR_RESET "\n", msg)

#define LOG_WARN(msg) \
    printf(ANSI_COLOR_YELLOW "[WARN] %s" ANSI_COLOR_RESET "\n", msg)

#define LOG_ERROR(msg) \
    printf(ANSI_COLOR_RED "[ERROR] %s" ANSI_COLOR_RESET "\n", msg)

// Log de debug con printf estilo variádico
#define DEBUG_PRINTF(fmt, ...) \
    printf(ANSI_COLOR_CYAN "[DEBUG] " fmt ANSI_COLOR_RESET "\n", ##__VA_ARGS__)

// Separador útil para secciones grandes
#define LOG_SEPARATOR() \
    printf(ANSI_COLOR_WHITE "\n------------------------------------------\n" ANSI_COLOR_RESET)

// Mostrar nombre y valor de una variable con tipo automático
#define LOG_VAR(var)                                                                                                     \
    do {                                                                                                                 \
        _Generic((var),                                                                                                  \
            int: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %d\n" ANSI_COLOR_RESET, (int)(var)),                        \
            unsigned int: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %u\n" ANSI_COLOR_RESET, (unsigned int)(var)),      \
            long: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %ld\n" ANSI_COLOR_RESET, (long)(var)),                     \
            unsigned long: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %lu\n" ANSI_COLOR_RESET, (unsigned long)(var)),   \
            float: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %.2f\n" ANSI_COLOR_RESET, (double)(var)),                 \
            double: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %.2lf\n" ANSI_COLOR_RESET, (double)(var)),               \
            char: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %c\n" ANSI_COLOR_RESET, (char)(var)),                      \
            char *: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %s\n" ANSI_COLOR_RESET, (char *)(var)),                  \
            const char *: printf(ANSI_COLOR_MAGENTA "[VAR] " #var " = %s\n" ANSI_COLOR_RESET, (const char *)(var)));     \
    } while (0)
