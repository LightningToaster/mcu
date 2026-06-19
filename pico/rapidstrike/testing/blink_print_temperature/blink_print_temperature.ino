#include "hardware/adc.h" // required for low-level adc functions
#define PIN_TEMPERATURE 4
#define NUM_SAMPLES 100  // number of samples to store
#define TEMP_MEASURE_INTERVAL 20  // measure every 10ms

float temp_buffer[NUM_SAMPLES]; // circular buffer to store temperature readings
int buffer_index = 0; // current index to store the next temperature reading
unsigned long last_measure_time = 0; // last time temperature was measured

unsigned long last_print_time = 0;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 200) {};

  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(PIN_TEMPERATURE);
}

void loop() {
  //measure temperature every 10ms
  if (millis() - last_measure_time >= TEMP_MEASURE_INTERVAL) {
    last_measure_time = millis();
    temp_buffer[buffer_index] = get_temperature();
    buffer_index = (buffer_index + 1) % NUM_SAMPLES;
  }

if (millis() - last_print_time >= 1000) {
    last_print_time = millis();
    Serial.print("temp: ");
    Serial.print(get_temperature(), 0);
    Serial.println(" °C");
  }
}

float get_temperature() {
  uint16_t raw = adc_read(); // 12-bit ADC result, 0–4095
  float voltage = raw * 3.3f / 4095.0f;
  float temp_c = 27.0f - (voltage - 0.706f) / 0.001721f;
  temp_c += 12.0f;  // calibration offset
  return temp_c;
}

float average_temperature() {
  float sum = 0.0f;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += temp_buffer[i];
  }
  return sum / NUM_SAMPLES;
}
