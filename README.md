# ⚙️ Guía de Configuración y Uso de ESP32 con ESP-IDF

## 📦 Requisitos

- Ubuntu (o distro basada en Linux)
- Python 3.7 o superior
- Git
- Paquetes: `cmake`, `ninja`, `pip`, `lsusb`, `minicom`

---

## 🚀 Paso 1: Instalar ESP-IDF

```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh
```

Agregar a tu shell (`.bashrc` o `.zshrc`):

```bash
echo 'source $HOME/esp/esp-idf/export.sh' >> ~/.bashrc
source ~/.bashrc
```

---

## 🧱 Paso 2: Crear nuevo proyecto

```bash
cd ~/esp
idf.py create-project barcodecheck
cd barcodecheck
```

O entra a un proyecto existente:

```bash
cd ~/esp/proyects/barcodecheck
```

---

## ⚙️ Paso 3: Configurar (opcional)

```bash
idf.py menuconfig
```

---

## 🧪 Paso 4: Compilar

```bash
idf.py build
```

---

## 🔌 Paso 5: Detectar puerto USB

Ver puertos disponibles:

```bash
ls /dev/ttyUSB*
```

O:

```bash
dmesg | grep tty
```

Alternativa con nombre amigable:

```bash
ls -l /dev/serial/by-id/
```

---

## 📤 Paso 6: Flashear firmware

```bash
idf.py -p /dev/ttyUSB0 flash
```

---

## 📡 Paso 7: Monitorear salida

```bash
idf.py -p /dev/ttyUSB0 monitor
```

También puedes combinar:

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

---

## 🛠️ Tip: Permisos para acceder a USB

```bash
sudo usermod -a -G dialout $USER
newgrp dialout
```

---

## 🔁 Script automático de flasheo

Crea un archivo `flash_auto.sh`:

```bash
#!/bin/bash
PORT=$(ls /dev/ttyUSB* 2>/dev/null | head -n 1)
if [ -z "$PORT" ]; then
    echo "❌ No se encontró ningún dispositivo /dev/ttyUSB"
else
    echo "✅ Puerto detectado: $PORT"
    idf.py -p "$PORT" flash monitor
fi
```

Hazlo ejecutable:

```bash
chmod +x flash_auto.sh
./flash_auto.sh
```

---

## 📁 Estructura recomendada del proyecto

```
barcodecheck/
├── CMakeLists.txt
├── main/
│   ├── app_main.c
│   └── CMakeLists.txt
├── components/
│   └── gm75_scanner/
│       ├── gm75_scanner.c
│       ├── gm75_scanner.h
│       └── CMakeLists.txt
├── build/
└── sdkconfig
```

---

## ✅ Recursos

- [Documentación oficial ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/index.html)
- [Component Registry](https://components.espressif.com/)

SPIFFS



# 📦 Módulo de Almacenamiento (storage)

Este módulo se encarga de guardar localmente los datos escaneados (por ejemplo, DNIs o códigos QR) cuando no hay conexión a internet, y de recuperar y eliminar los registros una vez enviados.

## 📁 Estructura

components/
└── storage/
├── storage.c
├── storage.h
└── CMakeLists.txt

csharp
Copiar
Editar

## ✅ Funcionalidades esperadas

- Inicializar SPIFFS.
- Guardar un registro (por ejemplo, DNI).
- Leer todos los registros pendientes.
- Eliminar los registros luego de enviarlos.
- Manejar errores y mostrar logs en caso de fallo.

## ✨ API esperada (`storage.h`)

```c
#pragma once

#include <stdbool.h>

bool storage_init(void);
bool storage_save_record(const char* record);
int  storage_get_all(char output[][64], int max_records);
bool storage_clear_all(void);
⚙️ Implementación básica (storage.c)
c
Copiar
Editar
#include <stdio.h>
#include <string.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include "storage.h"

#define STORAGE_FILE "/spiffs/pending.txt"
static const char* TAG = "Storage";

bool storage_init(void) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al montar SPIFFS");
        return false;
    }
    return true;
}

bool storage_save_record(const char* record) {
    FILE* f = fopen(STORAGE_FILE, "a");
    if (!f) {
        ESP_LOGE(TAG, "No se pudo abrir el archivo");
        return false;
    }
    fprintf(f, "%s\n", record);
    fclose(f);
    return true;
}

int storage_get_all(char output[][64], int max_records) {
    FILE* f = fopen(STORAGE_FILE, "r");
    if (!f) return 0;

    int count = 0;
    while (fgets(output[count], 64, f) && count < max_records) {
        // Eliminar salto de línea
        char* nl = strchr(output[count], '\n');
        if (nl) *nl = '\0';
        count++;
    }
    fclose(f);
    return count;
}

bool storage_clear_all(void) {
    return remove(STORAGE_FILE) == 0;
}
📌 CMakeLists.txt
cmake
Copiar
Editar
idf_component_register(SRCS "storage.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_spiffs esp_log)
💡 Uso típico
En sender_task, cuando se detecta que no hay red:

c
Copiar
Editar
if (!network_is_connected()) {
    storage_save_record(dni);
} else {
    send_to_api(dni);
}
Y cuando se recupere la conexión:

c
Copiar
Editar
char records[20][64];
int count = storage_get_all(records, 20);
for (int i = 0; i < count; ++i) {
    send_to_api(records[i]);
}
storage_clear_all();