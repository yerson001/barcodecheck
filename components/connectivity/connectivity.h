#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <stdbool.h>

// Retorna true si hay conexión a internet, false si no
bool connectivity_is_online(void);


void init_wifi(void);
#endif // CONNECTIVITY_H