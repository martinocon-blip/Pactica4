#include <Arduino.h>

// 1. Prototipo de la función (necesario para que setup la reconozca)
void anotherTask(void * parameter);

void setup() {
  Serial.begin(115200);
  
  // 2. Crear la tarea (asegurándose de que el nombre de la función sea correcto)
  xTaskCreate(
    anotherTask,     /* Función de la tarea */
    "anotherTask",   /* Nombre interno (sin espacios) */
    10000,           /* Tamaño de stack */
    NULL,            /* Parámetros */
    1,               /* Prioridad */
    NULL             /* Handle */
  ); 
}

void loop() {
  // El loop de Arduino es tratado por el ESP32 como una tarea más
  Serial.println("this is ESP32 Task");
  // En ESP32, delay() internamente llama a vTaskDelay, así que es seguro aquí
  delay(1000); 
}

// 3. Implementación de la función
void anotherTask(void * parameter) {
  for(;;) {
    Serial.println("this is another Task");
    // Es mejor práctica usar vTaskDelay en tareas de FreeRTOS
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
  
  // Aunque sea un bucle infinito, se incluye por seguridad
  vTaskDelete(NULL); 
}