#define SETUP_MESSAGE "BLINKOMETER_V2@RP2040 (Valkor 2026-1-27)" 

//TODO save data
//why batt percent only even numbers
#define PIN_USB_DETECT 1

#define PIN_BUTTON 12
#include "button.hpp"
Button button(PIN_BUTTON, 400);//ms for flicker counter reset

#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9
#include "oled.hpp"
Display oled(PIN_OLED_SDA, PIN_OLED_SCL);

#include "nitsmeter.hpp"
NitsMeter sensor1(26); //TODO use other sensor?

#include "buzzer.hpp"
#define PIN_BUZZER 0
Buzzer buzzer(PIN_BUZZER);

#include "voltmeter.hpp"
Voltmeter voltmeter(29);

static uint32_t flicker_count = 0;
static uint32_t last_stopwatch_ms = 0;
static uint32_t stopwatch_seconds = 0;
static bool paused = true;

void setup() {
    Serial.begin(9600);
    oled.begin();
    buzzer.effect_turnon();
    pinMode(PIN_USB_DETECT, INPUT_PULLDOWN);
}//setup

void flicker(uint16_t a = 1){
	if (a == 1){ //if no input, just increment
		flicker_count++;
	}else if (a == 0){//if input is 0, reset
		flicker_count = 0;
	}
	oled.set_flicker(flicker_count);
}//flicker

void loop() {
    bool USB_connected = digitalRead(PIN_USB_DETECT); 

    if (voltmeter.get_status() == BATTERY_LOW and not USB_connected) {
       shutdown();
    }
    voltmeter.operate();
    if (button.operate() == BUTTON_HOLD){
        if (flicker_count == 0){
            paused = !paused;
            buzzer.effect_beep();
            oled.set_pause(USB_connected);//paused
        }else{
            flicker(0);
            buzzer.effect_reset();
            stopwatch_seconds = 0;
            last_stopwatch_ms = millis(); // reset timer reference 
        }
        
    }//button held
    buzzer.operate(flicker_count > 0);
    sensor1.operate();
    uint16_t adc = sensor1.get_adc();
    oled.set_adc_channel(1);        
    if (sensor1.get_flicker() > 0 and !paused){
        flicker();
        buzzer.effect_flicker();
    }//flicker detected

    uint32_t now = millis();
    if (now - last_stopwatch_ms >= 1000) {
        last_stopwatch_ms += 1000;  // advance exactly 1 second per tick
        stopwatch_seconds++;
    }
    uint8_t seconds = stopwatch_seconds % 60;
    uint8_t minutes = (stopwatch_seconds / 60) % 60;
    uint16_t hours = stopwatch_seconds / 3600;

    oled.set_adc_value(adc);
    oled.set_battery(voltmeter.get_string(PERCENTAGE));
    oled.set_seconds(seconds);
    oled.set_minutes(minutes);
    oled.set_hours(hours);
    oled.set_USB(USB_connected);
    
    oled.update();

}//loop

void shutdown(){
    oled.battery_died();      
    digitalWrite(LED_BUILTIN, LOW);
    noTone(PIN_BUZZER);   // stop buzzer
    //TODO save data
    while(true) {
        __wfi();          // halt core0 indefinitely
    }
}

