// Valkor 2024-10-20

#define TEMPERATURE_UPDATE_INTERVAL 10000 //ms
#define MS_TO_SUBTRACT_SECOND 5715000 //  7.2mil was too much

#define ONE_WIRE_BUS 3 // DS18B20 temperature sensor
#include <Wire.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
RTC_DS1307 rtc;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

class Time{ 
public:
  Time(){
    sensors.begin();
    if (!rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }

    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    sensors.setResolution(12);
    sensors.requestTemperatures();//be ready for first request
    temperatureC = sensors.getTempCByIndex(0); 
  }//constructor

  void operate_temperature(){
    if (millis() > ms + TEMPERATURE_UPDATE_INTERVAL/2){
      if (is_temperature_requested){
        temperatureC = sensors.getTempCByIndex(0); 
        is_temperature_requested = false;
      }else{
        sensors.requestTemperatures();
        is_temperature_requested = true;
      }
      ms = millis();
    }

    
  }//operate_temperature

  void operate_adjustment(){
    #ifdef MS_TO_SUBTRACT_SECOND
    if (millis() >= ms_2 + MS_TO_SUBTRACT_SECOND){
      ms_2 = millis();
      DateTime now = rtc.now();
      DateTime adjustedTime = now - TimeSpan(0, 0, 0, 1); // Subtract 1 second (TimeSpan(days, hours, minutes, seconds))
      rtc.adjust(adjustedTime);
    }
    #endif
  }
  
  int8_t get_hour(){
    DateTime now = rtc.now();
    return now.hour();
  }//get_hour

  int8_t get_minute(){
    DateTime now = rtc.now();
    return now.minute();
  }//get_minute

  int8_t get_second(){
    DateTime now = rtc.now();
    return now.second();
  }//get_second

  float get_temperature(){
    return temperatureC + TEMPERATURE_ADJUSTMENT;
  }//get_temperature

  void add_minutes(int8_t minutes_to_add){
    DateTime now = rtc.now();
    int8_t new_minute = now.minute() + minutes_to_add;
    int8_t new_hour = now.hour();

    if (new_minute >= 60) {
      new_minute -= 60;
      new_hour++;
      if (new_hour >= 24) {
        new_hour -= 24;
      }
    }
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), new_hour, new_minute, now.second()));
  }


private:
  float temperatureC = 99;
  unsigned long ms = 0;
  unsigned long ms_2 = 0;
  bool is_temperature_requested = false;
};//class
