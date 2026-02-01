///DISABLED!!

#include "hardware/adc.h" // required for low-level adc functions

class Temperature{ 
  private:
  static constexpr uint8_t TEMP_SENSOR_ADC_CHANNEL = 4;
  static constexpr uint8_t NUM_SAMPLES = 100;
  static constexpr uint8_t TEMP_MEASURE_INTERVAL = 250;
  float temp_buffer[NUM_SAMPLES]; //circular buffer
  uint8_t buffer_index = 0;
  unsigned long ms = 0;


  public:

  Temperature(){
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(TEMP_SENSOR_ADC_CHANNEL);
  }

  void operate(){
    if (millis()-ms < TEMP_MEASURE_INTERVAL){return;}ms = millis();
    temp_buffer[buffer_index] = read_temperature();
    buffer_index = (buffer_index + 1) % NUM_SAMPLES;    
  }

  float read_temperature() {
    uint16_t raw = adc_read(); // 12-bit ADC result, 0–4095
    Serial.print(raw); Serial.print("\t\t");
    float voltage = raw * 3.3f / 4095.0f;

  

    

    float temp_c = 39.0f - (voltage - 0.706f) / 0.001721f;

    Serial.println(temp_c, 3);
    return temp_c;
}


  float get() {
    float sum = 0.0f;
    for (int i = 0; i < NUM_SAMPLES; i++) {
      sum += temp_buffer[i];
    }
    return sum / NUM_SAMPLES;
  }
 

};//Temperature