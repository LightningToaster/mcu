// Valkor 2024-08-03

const uint8_t POWERS[32] = {0,1,2,3,4 // (0-4)
  ,5,6,7,8,10 // (5-9)
  ,12,14,16,18,20 // (10-14) 
  ,25,30,35,40,50 // (15-19)
  ,60,70,80,90,110 // (20-24)
  ,130,150,170,190,210 // (25-29)
  ,230,255 // (30-31)
}; // v3

#define TRANSITION_DELAY 5//20

#define DIODES_PER_PIXEL 3
#include <Adafruit_NeoPixel.h>

class Strip{
  
public:
  Strip(uint8_t pin, uint8_t num_pixels){
    this->pin = pin;
    this->num_pixels = num_pixels;
    for (uint8_t pixel = 0; pixel < NUM_PIXELS; pixel++){
      for (uint8_t diode = 0; diode < DIODES_PER_PIXEL; diode++){
        goals[pixel][diode] = 0;
      }//for each diode in package
    }//for each LED package in assembly
  }//constructor

  uint8_t operate(bool wait_for_completion = false){//returns how many chips haven't reached their goal + 1
    if (not setup_complete){
      setup();
    }
    uint8_t status = 0;
    if (millis() - last_update_ms > TRANSITION_DELAY){
      
      for (uint8_t chip_i = 0; chip_i<NUM_PIXELS; chip_i++){
        uint32_t c = strip->getPixelColor(chip_i);
        uint8_t r = c >> 16;
        uint8_t g = c >> 8;
        uint8_t b = c;

        r = find_power_index(r);
        g = find_power_index(g);
        b = find_power_index(b);//index based from here on
  
        if (r < goals[chip_i][0]){ r++; 
        }else if (r > goals[chip_i][0]){ r--; }
  
        if (g < goals[chip_i][1]){ g++; 
        }else if (g > goals[chip_i][1]){ g--; }
  
        if (b < goals[chip_i][2]){ b++; 
        }else if (b > goals[chip_i][2]){ b--; }

        if (r != goals[chip_i][0]
        or g != goals[chip_i][1]
        or b != goals[chip_i][2]){
          status++;
        }
        strip->setPixelColor(chip_i, POWERS[r], POWERS[g], POWERS[b]);
      }//for each diode in package
      last_update_ms = millis();
      strip->show();
      //if (wait_for_completion and status == 1){
      //  status = 0;
     // }
    }//if
    if (status > 0 and wait_for_completion){
      return operate(true);
    }
    return status;
  }//operate

  void set_all(uint8_t r, uint8_t g, uint8_t b){
    for (uint8_t led = 0; led < NUM_PIXELS; led++){
      set(led,r,g,b);
    }
  }//set_all

  void set(int8_t led, uint8_t r, uint8_t g, uint8_t b){
    if (led>=num_pixels){
      led = led%num_pixels;
    }
    while (led<0){
      led = num_pixels;
    }
    goals[led][0] = r;
    goals[led][1] = g;
    goals[led][2] = b;
  }//set
  
private:
  Adafruit_NeoPixel* strip;
  bool setup_complete = false;
  uint8_t goals[NUM_PIXELS][DIODES_PER_PIXEL];
  unsigned long last_update_ms = 0;
  uint8_t pin;
  uint8_t num_pixels;

  void setup(){
    strip = new Adafruit_NeoPixel(NUM_PIXELS, this->pin, NEO_GRB + NEO_KHZ800);
    strip->begin();
    strip->show();//initialized to off
    setup_complete = true;
  }//setup
  
  uint8_t find_power_index(uint8_t value){
    for (uint8_t i = 0; i < sizeof(POWERS)/sizeof(POWERS[0]); i++){
      if (POWERS[i] == value){
        return i;
      }
    }
    return find_power_index(value-1);
  }//find_power_index

};//class
