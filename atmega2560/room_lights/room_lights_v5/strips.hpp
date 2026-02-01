
const uint8_t POWERS[32] = {0,1,2,3,4 // (0-4)
  ,5,6,7,8,10 // (5-9)
  ,12,14,16,18,20 // (10-14) 
  ,25,30,35,40,50 // (15-19)
  ,60,70,80,90,110 // (20-24)
  ,130,150,170,190,210 // (25-29)
  ,230,255 // (30-31)
}; // v3

#define ZONE_ALL 0
#define ZONE_DESK 1
#define ZONE_WINDOW 2

#include <Adafruit_NeoPixel.h>
#define PIXELS_PER_STRIP 20
#define COLOR_CHANNELS_PER_PIXEL 4
#define NUM_STRIPS 8
#define uint8_t u8

class Strips{ public:

  Strips(){
    for (uint8_t strip = 0; strip<NUM_STRIPS; strip++){
      strips[strip].begin();
      strips[strip].setBrightness(255);
      for (uint8_t led = 0; led<PIXELS_PER_STRIP; led++){
        strips[strip].setPixelColor(led,0,0,0,0);
        goals[strip][led][0] = 0;
        goals[strip][led][1] = 0;
        goals[strip][led][2] = 0;
        goals[strip][led][3] = 0;
      }
      strips[strip].show();// Initialize all pixels
    }//for
  }//Strips()

  int8_t operate(){
    if (millis() - transition_ms >= transition_delay){
      transition_ms = millis();
      return transition();
    }
    return 0;
  }//operate
  
  int16_t transition(){
    int16_t status = 0;
    
    for (uint8_t i_strip = 0; i_strip<NUM_STRIPS; i_strip++){

      for (uint8_t i_pixel = 0; i_pixel<PIXELS_PER_STRIP; i_pixel++){
        uint32_t c = strips[i_strip].getPixelColor(i_pixel);
        uint8_t r = c >> 16;
        uint8_t g = c >> 8;
        uint8_t b = c;
        uint8_t w = c >> 24;//for RGBW

        r = find_power_index(r);
        g = find_power_index(g);
        b = find_power_index(b);
        w = find_power_index(w);//index based from here on

        if (r != goals[i_strip][i_pixel][0]
        or g != goals[i_strip][i_pixel][1]
        or b != goals[i_strip][i_pixel][2]
        or w != goals[i_strip][i_pixel][3]){
          status++;
        }
        
        if (r < goals[i_strip][i_pixel][0]){ r++; 
        }else if (r > goals[i_strip][i_pixel][0]){ r--; }
          
        if (g < goals[i_strip][i_pixel][1]){ g++; 
        }else if (g > goals[i_strip][i_pixel][1]){ g--; }
  
        if (b < goals[i_strip][i_pixel][2]){ b++; 
        }else if (b > goals[i_strip][i_pixel][2]){ b--; }
  
        if (w < goals[i_strip][i_pixel][3]){ w++; 
        }else if (w > goals[i_strip][i_pixel][3]){ w--; }

        if (index_zone == ZONE_ALL){
          strips[i_strip].setPixelColor(i_pixel, POWERS[r], POWERS[g], POWERS[b], POWERS[w]);
        }else if (index_zone == ZONE_DESK){       
          if (i_strip > 2 ){
            strips[i_strip].setPixelColor(i_pixel, 0, 0, 0, 0);
            strips[i_strip].setPixelColor(0, 0, 0, 1, 0);
          }else{
            strips[i_strip].setPixelColor(i_pixel, POWERS[r], POWERS[g], POWERS[b], POWERS[w]);
          }
        }else if(index_zone == ZONE_WINDOW){
          if (i_strip < 4 or i_strip > 6 ){
            strips[i_strip].setPixelColor(i_pixel, 0, 0, 0, 0);
            strips[i_strip].setPixelColor(0, 0, 0, 1, 0);
          }else{
            strips[i_strip].setPixelColor(i_pixel, POWERS[r], POWERS[g], POWERS[b], POWERS[w]);
          }
        }
        
        
      }//each led
    }//each strip

    if (status > 0){
      for (uint8_t i_strip = 0; i_strip < NUM_STRIPS; i_strip++){
        strips[i_strip].show();
      }
    }
    
    return status;
    
  }//transition

  uint8_t find_power_index(uint8_t value){
    for (uint8_t i = 0; i < sizeof(POWERS)/sizeof(POWERS[0]); i++){
      if (POWERS[i] == value){
        return i;
      }//if
    }//for
    return find_power_index(value-1);
  }//find_power_index


  void set(u8 strip, u8 pixel, u8 r, u8 g, u8 b, u8 w){
    strips[strip].setPixelColor(pixel, POWERS[r], POWERS[g], POWERS[b], POWERS[w]);
  }//set

  void set_goal(u8 strip, u8 pixel, u8 r, u8 g, u8 b, u8 w){
    goals[strip][pixel][0] = r;
    goals[strip][pixel][1] = g;
    goals[strip][pixel][2] = b;
    goals[strip][pixel][3] = w;
  }//set_goal
  
  void set_goal_all(u8 r, u8 g, u8 b, u8 w, u8 increment = 1){
    for (uint8_t strip = 0; strip < NUM_STRIPS; strip++){
      for (int8_t pixel = PIXELS_PER_STRIP-1; pixel>=0; pixel-=increment){
        set_goal(strip,pixel,r,g,b,w);
      }
    }
  }//end set_goal_all

  void set_goal_ring(u8 pixel, u8 r, u8 g, u8 b, u8 w){
    for (uint8_t strip = 0; strip < NUM_STRIPS; strip++){
      set_goal(strip,pixel,r,g,b,w);
    }
  }//end set_goal_ring

  void next_zone(){
    index_zone = (index_zone+1)%3;
    set_goal_all(0,0,0,0);
    
  }

  private:
  Adafruit_NeoPixel strips[NUM_STRIPS] = {                         
    Adafruit_NeoPixel(PIXELS_PER_STRIP, 29, NEO_RGBW + NEO_KHZ800),// 0 = door
    Adafruit_NeoPixel(PIXELS_PER_STRIP, 27, NEO_RGBW + NEO_KHZ800),// 1 = computer
    Adafruit_NeoPixel(PIXELS_PER_STRIP, 25, NEO_RGBW + NEO_KHZ800),// 2 = bathroom
    Adafruit_NeoPixel(PIXELS_PER_STRIP, 24, NEO_RGBW + NEO_KHZ800),// 3 = bed
    Adafruit_NeoPixel(PIXELS_PER_STRIP, 26, NEO_RGBW + NEO_KHZ800),// 4 = pillow
    Adafruit_NeoPixel(PIXELS_PER_STRIP, 28, NEO_RGBW + NEO_KHZ800),// 5 = window
    Adafruit_NeoPixel(PIXELS_PER_STRIP, 30, NEO_RGBW + NEO_KHZ800),// 6 = drawers
    Adafruit_NeoPixel(PIXELS_PER_STRIP, 31, NEO_RGBW + NEO_KHZ800) // 7 = closet
  };
  uint8_t transition_delay = 5; //delay between increments towards goals
  unsigned long transition_ms = 0;
  uint8_t goals[NUM_STRIPS][PIXELS_PER_STRIP][COLOR_CHANNELS_PER_PIXEL];
  uint8_t index_zone = ZONE_ALL;
};//class Strips
