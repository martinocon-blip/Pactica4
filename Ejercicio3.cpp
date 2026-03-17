#include <Arduino.h>
#include "FreeRTOS.h"
#include "freertos/task.h"

const int led1 = 2;   // 1 Hz
const int led2 = 21;   // 3.33 Hz

// LED 1 → Core 0
void taskLED1(void * parameter) {
  for(;;) {
    digitalWrite(led1, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    digitalWrite(led1, LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// LED 2 → Core 1
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

  // LED1 en Core 0
  xTaskCreatePinnedToCore(
    taskLED1,
    "LED1 Task",
    1000,
    NULL,
    1,
    NULL,
    0
  );

  // LED2 en Core 1
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
  // vacío
}