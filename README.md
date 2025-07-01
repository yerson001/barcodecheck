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

¡Buena observación!  
En un diseño eficiente y simple, **no necesitas una tarea extra solo para verificar almacenamiento o conectividad**. El flujo puede ser así:

- El **scanner** pone datos en la cola.
- El **sender** toma de la cola y:
  - Si hay internet, envía.
  - Si no hay, guarda en storage.

Luego, el **sender** puede, cada cierto tiempo, revisar si hay datos guardados y si ya hay internet, los envía.  
Así, solo necesitas dos tareas principales:  
1. **scanner_task** (escaneo y push a la cola)  
2. **sender_task** (consume la cola, decide enviar o guardar, y reintenta lo guardado si hay red)

Esto mantiene el sistema simple y eficiente, usando bien las colas de FreeRTOS.  
¿Quieres un ejemplo de cómo quedaría ese sender_task?


[gm75_scanner] 
      |
      v
   [queue] 
      |
      v
   [cipher] (opcional)
      |
      v
   [sender] ---(consulta)---> [connectivity]
      |                             |
      |<---¿hay internet?---Sí------+
      |                             |
      |---No------------------------+
      v
 [storage] <---+
      ^        |
      |        |
      +--------+
  (sender revisa storage periódicamente y reintenta si hay internet)



  #include <stdio.h>
#include "gm75_scanner.h"
#include "esp_log.h"
#include "logger.h"

void app_main(void) {
    ESP_LOGI("APP", "Iniciando ColeCheck Scanner...");
    LOG_SEPARATOR();
    LOG_INFO("hola");
    LOG_SEPARATOR();
    //gm75_scanner_init();
    //gm75_scanner_start();
}

