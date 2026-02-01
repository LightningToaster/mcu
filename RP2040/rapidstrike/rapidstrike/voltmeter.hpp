class Voltmeter{ 

  private:
    static constexpr float VOLTAGE_MIN_4S = 14.0;// 3.5v x4
    static constexpr float VOLTAGE_CHARGED_4S = 16.4;//4.1v x4
    static constexpr float VOLTAGE_MAX_4S = 17.2;// 4.3v x4
    static constexpr float REFERENCE_VOLTAGE = 3.24;
    static constexpr float R1 = 47000.0;
    static constexpr float R2 = 10000.0;
    static constexpr uint8_t NUM_SAMPLES = 20;
    static constexpr uint8_t MS_BETWEEN_READS = 50;
    uint8_t pin;
    uint16_t analog_voltages[NUM_SAMPLES];
    

  public:
  Voltmeter(uint8_t pin){
    this->pin = pin;
    pinMode(pin, INPUT);
    for (int i=0; i<NUM_SAMPLES; i++){
      analog_voltages[i] = 0;
    }
  }

  bool operate(){
    static unsigned long ms = 0;
    static uint8_t buffer_index = 0;
    if (millis()-ms < MS_BETWEEN_READS){return is_ok();}ms = millis();
    analog_voltages[buffer_index] = analogRead(pin);
    buffer_index = (buffer_index + 1) % NUM_SAMPLES;

    return is_ok();
  }

  float get(){
    int average = 0;
    for (int i = 0; i<NUM_SAMPLES; i++){
      if (analog_voltages[i] < 1){return 0.0;}
      average += analog_voltages[i];
    }
    average/=NUM_SAMPLES;
    
    float voltage = (average / 1023.0) * REFERENCE_VOLTAGE;// Convert analog value to voltage
    voltage = (voltage * (R1 + R2)) / R2;// Calculate LiPo voltage using voltage divider formula
    return voltage;
  }

  char* get_string() {
    static char str[8];  // enough space for "x.xxv\0"
    float lipo_voltage = get();

    if (lipo_voltage == 0.0) {
        return (char*)"0v";
    }

    float cell_voltage = 0.0;
    if  (is_ok()) {
        cell_voltage = lipo_voltage / 4.0;
    } else {
        return (char*)"0v";  // unhandled voltage
    }

    dtostrf(cell_voltage, 3, 2, str);
    strcat(str, "v");
    return str;
  }

  bool is_ok(){
    float v = get();
    return (v >= VOLTAGE_MIN_4S and v <= VOLTAGE_MAX_4S);
  }
};//Voltmeter