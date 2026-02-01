class Indicator_Led{
  public:

  void begin(){
    strip.begin();
    strip.setBrightness(255);
    strip.setPixelColor(0,0,0,0);
    strip.show();
    has_begun = true;
  }
  uint8_t operate(){
    if (not has_begun){ begin();}
    int16_t status = 0;
    
    uint32_t c = strip.getPixelColor(0);
    uint8_t r = c >> 16;
    uint8_t g = c >> 8;
    uint8_t b = c;
    
    if (r != goal[0]
    or g != goal[1]
    or b != goal[2]){
        status++;
    }
        
    if (r < goal[0]){ r++; 
    }else if (r > goal[0]){ r--; }
      
    if (g < goal[1]){ g++; 
    }else if (g > goal[1]){ g--; }

    if (b < goal[2]){ b++; 
    }else if (b > goal[2]){ b--; }

    strip.setPixelColor(0, r, g, b);

    if (status > 0){  
      strip.show();
    }
    return status;
  }//operate

  void set_color(uint8_t r, uint8_t g, uint8_t b){
    goal[0] = r;
    goal[1] = g;
    goal[2] = b;
  }//set_color
  
  private:
    bool has_begun = false;
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN_INDICATOR_LED, NEO_GRB + NEO_KHZ800);//num, pin
    uint8_t goal[3] = {0,0,0};
    
};//class Indicator_Led
