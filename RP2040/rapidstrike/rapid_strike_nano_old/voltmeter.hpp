#define VOLTAGE_MIN_3S 10.5
#define VOLTAGE_MAX_3S 12.9
#define VOLTAGE_MIN_4S 14.0
#define VOLTAGE_MAX_4S 17.2

#define R1 27000.0
#define R2 10000.0
#define REFERENCE_VOLTAGE 5.04 // it *should* about 5V reference for the analog pin
#define MS_BETWEEN_READS 10
#define NUM_SAMPLES 20

class Voltmeter{ 
  public:

 
  void begin(){
    pinMode(PIN_VOLTAGE, INPUT);
    for (int i=0; i<NUM_SAMPLES; i++){
      analog_voltages[i] = 0;
    }
    begun = true;
  }

  void operate(){
    if (not begun) {begin();}
    if (millis()-ms < MS_BETWEEN_READS){return;}

    ms = millis();
    
    for (int i=NUM_SAMPLES-1; i>0; i--){ //shuffle everything up one
      analog_voltages[i] = analog_voltages[i-1];
    }
    analog_voltages[0] = analogRead(PIN_VOLTAGE);//overwrite position 0
  }

  float get(){
    int average = 0;
    for (int i = 0; i<NUM_SAMPLES; i++){
      if (analog_voltages[i] < 1){return 0.0;}
      average += analog_voltages[i];
    }
    average/=NUM_SAMPLES;
    
    float voltage= (average / 1023.0) * REFERENCE_VOLTAGE;// Convert analog value to voltage
    voltage = (voltage * (R1 + R2)) / R2;// Calculate LiPo voltage using voltage divider formula

    return voltage;
  }

  char* get_string(){
    float lipo_voltage = get();
    if (lipo_voltage == 0.0){
      return "?v";
    }

    float cell_voltage;
    if (lipo_voltage >= VOLTAGE_MIN_3S and lipo_voltage <= VOLTAGE_MAX_3S){
      cell_voltage = lipo_voltage/3;
    }else if (lipo_voltage >= VOLTAGE_MIN_4S and lipo_voltage <= VOLTAGE_MAX_4S){
      cell_voltage = lipo_voltage/4;
    }
    dtostrf(cell_voltage, 3, 2, str);  // 4 is the total width including the decimal point
    strcat(str, "v");
    return str;
  }

  bool is_ok(){
    float v = get();
    return ((v >= VOLTAGE_MIN_3S and v <= VOLTAGE_MAX_3S) 
    or (v >= VOLTAGE_MIN_4S and v <= VOLTAGE_MAX_4S));
  }

  private:
  bool begun = false;
  uint16_t analog_voltages[NUM_SAMPLES];
  unsigned long ms = 0;
  char str[10];    // Buffer to hold the formatted LiPo voltage string



};//Voltmeter


