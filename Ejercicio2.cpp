#include <Arduino.h>

// 1. Declaración de funciones (Prototipos) para que el compilador las conozca
void taskEncender(void *pvParameters);
void taskApagar(void *pvParameters);

// Pin del LED (GPIO 2 suele ser el LED interno del ESP32) [cite: 23]
const int ledPin = 2;

// Manejador del semáforo
SemaphoreHandle_t xSemaphore;

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);

    // Creamos el semáforo binario 
    xSemaphore = xSemaphoreCreateBinary();

    // Damos el semáforo por primera vez para que la tarea de encender pueda arrancar
    xSemaphoreGive(xSemaphore);

    // Creamos las dos tareas [cite: 49, 152]
    xTaskCreate(taskEncender, "Encender", 2048, NULL, 1, NULL);
    xTaskCreate(taskApagar, "Apagar", 2048, NULL, 1, NULL);
}

void loop() {
    // El loop se queda vacío porque FreeRTOS gestiona las tareas [cite: 173]
}

// Tarea para ENCENDER el LED
void taskEncender(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            digitalWrite(ledPin, HIGH);
            Serial.println("LED: ENCENDIDO (Tarea 1)");
            
            // Pausa la tarea (500ms o lo que prefieras) [cite: 35, 45]
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            
            // "Suelta" el semáforo para que la Tarea de Apagar pueda ejecutarse
            xSemaphoreGive(xSemaphore);
            
            // Pequeña pausa para permitir que el planificador cambie de tarea [cite: 46]
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

// Tarea para APAGAR el LED
void taskApagar(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            digitalWrite(ledPin, LOW);
            Serial.println("LED: APAGADO (Tarea 2)");
            
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            
            // "Suelta" el semáforo para que la Tarea de Encender pueda volver a actuar
            xSemaphoreGive(xSemaphore);
            
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}