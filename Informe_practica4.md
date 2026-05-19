# Práctica 4 - Sistemas Operativos en Tiempo Real (FreeRTOS)

## Objetivo

El objetivo de esta práctica es comprender el funcionamiento de un sistema operativo en tiempo real (RTOS) utilizando FreeRTOS sobre ESP32.

Para ello se han implementado diferentes ejercicios donde múltiples tareas se ejecutan de forma concurrente utilizando:

- multitarea,
- semáforos,
- sincronización,
- planificación de CPU,
- y ejecución multinúcleo.

La práctica permite observar cómo FreeRTOS distribuye el tiempo de procesamiento entre diferentes tareas.

---

# Introducción teórica

FreeRTOS es un sistema operativo en tiempo real integrado en el ESP32 que permite ejecutar múltiples tareas simultáneamente.

Cada tarea puede:

- ejecutarse de forma independiente,
- tener distinta prioridad,
- utilizar retardos no bloqueantes,
- sincronizarse mediante semáforos,
- e incluso ejecutarse en núcleos específicos del procesador.

El ESP32 dispone de dos núcleos, permitiendo ejecutar tareas en paralelo.

Las funciones principales utilizadas en esta práctica son:

```cpp
xTaskCreate()
```

Permite crear tareas gestionadas por FreeRTOS.

```cpp
vTaskDelay()
```

Suspende temporalmente una tarea sin bloquear el sistema.

```cpp
xTaskCreatePinnedToCore()
```

Permite ejecutar una tarea en un núcleo concreto.

```cpp
SemaphoreHandle_t
```

Permite sincronizar tareas utilizando semáforos.

---

# Ejercicio 1 - Multitarea básica

## Objetivo

Crear dos tareas que se ejecuten simultáneamente:

- la tarea principal (`loop`)
- una tarea adicional creada mediante FreeRTOS.

---

# Código implementado

```cpp
#include <Arduino.h>

void anotherTask(void * parameter);

void setup() {

  Serial.begin(115200);

  xTaskCreate(
    anotherTask,
    "anotherTask",
    10000,
    NULL,
    1,
    NULL
  );
}

void loop() {

  Serial.println("this is ESP32 Task");

  delay(1000);
}

void anotherTask(void * parameter) {

  for(;;) {

    Serial.println("this is another Task");

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}
```

---

# Funcionamiento

Durante la ejecución:

- `loop()` actúa como una tarea gestionada por FreeRTOS.
- `anotherTask()` se ejecuta simultáneamente.
- Ambas tareas imprimen mensajes por puerto serie cada segundo.

El sistema operativo alterna automáticamente la ejecución entre ambas tareas.

---

# Salida por puerto serie

```text
this is ESP32 Task
this is another Task
this is ESP32 Task
this is another Task
```

---

# Explicación

La función:

```cpp
xTaskCreate()
```

crea una nueva tarea administrada por FreeRTOS.

El uso de:

```cpp
vTaskDelay()
```

permite liberar la CPU temporalmente para que otras tareas puedan ejecutarse.

Sin este mecanismo, una tarea podría monopolizar completamente el procesador.

---

# Ejercicio 2 - Sincronización mediante semáforos

## Objetivo

Implementar dos tareas sincronizadas:

- una tarea enciende un LED,
- otra tarea lo apaga.

La sincronización se realiza mediante un semáforo binario.

---

# Código implementado

```cpp
#include <Arduino.h>

void taskEncender(void *pvParameters);
void taskApagar(void *pvParameters);

const int ledPin = 2;

SemaphoreHandle_t xSemaphore;

void setup() {

    Serial.begin(115200);

    pinMode(ledPin, OUTPUT);

    xSemaphore = xSemaphoreCreateBinary();

    xSemaphoreGive(xSemaphore);

    xTaskCreate(taskEncender, "Encender", 2048, NULL, 1, NULL);

    xTaskCreate(taskApagar, "Apagar", 2048, NULL, 1, NULL);
}

void loop() {

}

void taskEncender(void *pvParameters) {

    for (;;) {

        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {

            digitalWrite(ledPin, HIGH);

            Serial.println("LED: ENCENDIDO (Tarea 1)");

            vTaskDelay(1000 / portTICK_PERIOD_MS);

            xSemaphoreGive(xSemaphore);

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

void taskApagar(void *pvParameters) {

    for (;;) {

        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {

            digitalWrite(ledPin, LOW);

            Serial.println("LED: APAGADO (Tarea 2)");

            vTaskDelay(1000 / portTICK_PERIOD_MS);

            xSemaphoreGive(xSemaphore);

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}
```

---

# Funcionamiento

El semáforo garantiza que únicamente una tarea pueda controlar el LED al mismo tiempo.

Secuencia:

1. La tarea `taskEncender()` obtiene el semáforo.
2. Enciende el LED.
3. Espera 1 segundo.
4. Libera el semáforo.
5. La tarea `taskApagar()` obtiene el semáforo.
6. Apaga el LED.
7. Espera 1 segundo.
8. Libera nuevamente el semáforo.

Esto evita conflictos entre tareas.

---

# Salida serie

```text
LED: ENCENDIDO (Tarea 1)
LED: APAGADO (Tarea 2)
LED: ENCENDIDO (Tarea 1)
LED: APAGADO (Tarea 2)
```

---

# Explicación del semáforo

El semáforo actúa como un mecanismo de control de acceso.

Funciones utilizadas:

```cpp
xSemaphoreTake()
```

Solicita el acceso al recurso compartido.

```cpp
xSemaphoreGive()
```

Libera el recurso para otra tarea.

---

# Ejercicio 3 - Multinúcleo en ESP32

## Objetivo

Ejecutar tareas diferentes en núcleos distintos del ESP32.

- LED1 → Core 0
- LED2 → Core 1

---

# Código implementado

```cpp
#include <Arduino.h>
#include "FreeRTOS.h"
#include "freertos/task.h"

const int led1 = 2;
const int led2 = 21;

void taskLED1(void * parameter) {

  for(;;) {

    digitalWrite(led1, HIGH);

    vTaskDelay(pdMS_TO_TICKS(500));

    digitalWrite(led1, LOW);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void taskLED2(void * parameter) {

  for(;;) {

    digitalWrite(led2, HIGH);

    vTaskDelay(pdMS_TO_TICKS(150));

    digitalWrite(led2, LOW);

    vTaskDelay(pdMS_TO_TICKS(150));
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(led1, OUTPUT);

  pinMode(led2, OUTPUT);

  xTaskCreatePinnedToCore(
    taskLED1,
    "LED1 Task",
    1000,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    taskLED2,
    "LED2 Task",
    1000,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {

}
```

---

# Funcionamiento

En este ejercicio:

- el LED1 parpadea a 1 Hz,
- el LED2 parpadea aproximadamente a 3.33 Hz.

Cada tarea se ejecuta en un núcleo distinto del ESP32.

Esto demuestra la capacidad multinúcleo del microcontrolador y la capacidad de FreeRTOS para distribuir tareas entre ambos procesadores.

---

# Explicación de xTaskCreatePinnedToCore()

```cpp
xTaskCreatePinnedToCore()
```

Permite fijar una tarea a un núcleo específico.

Parámetros importantes:

- función de la tarea,
- nombre,
- stack,
- prioridad,
- núcleo destino.

---

# Ventajas de FreeRTOS

- Multitarea real.
- Mejor aprovechamiento de CPU.
- Ejecución concurrente.
- Sincronización segura.
- Gestión eficiente del tiempo.
- Soporte multinúcleo.

---

# Conclusiones

En esta práctica se ha aprendido:

- Cómo crear tareas en FreeRTOS.
- Cómo utilizar `vTaskDelay()`.
- Cómo sincronizar tareas mediante semáforos.
- Cómo evitar conflictos entre tareas.
- Cómo ejecutar tareas en núcleos específicos.
- Cómo aprovechar la arquitectura dual-core del ESP32.

Además, se ha comprobado cómo FreeRTOS permite dividir eficientemente el tiempo de CPU entre múltiples procesos concurrentes.
