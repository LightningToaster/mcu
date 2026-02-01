//uses A4 (SDA) & A5 (CLK)

//TODO pretty much delete all this



//#include <GyverOLED.h>
//GyverOLED<SSD1306_128x32> display;

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 64 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


class Oled{ 
  public:

  Oled(){}
 
  void begin(){//might be better to use constructor?
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay(); // cleanup from last run
    //display.drawBitmap(0, 0, bitmap_Aperture_Laboratories_Logo, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    int ammo = 20;
    output(String(ammo));
    delay(500);   
    //display.display(); 

    // display.setTextSize(4);
    // display.setTextColor(WHITE);
    // delay(500);

  }//begin

void output(const String& text) {
  display.clearDisplay();
  display.setTextSize(4); // Adjust the text size as needed
  display.setTextColor(WHITE); // Adjust the text color as needed

  // Get the width and height of the text
  int16_t textWidth, textHeight, x1, y1;
  display.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);

  // Calculate the position to center the text
  int16_t xPos = (SCREEN_WIDTH - textWidth) / 2;
  int16_t yPos = (SCREEN_HEIGHT - textHeight) / 2;

  // Set the cursor position to the centered position
  display.setCursor(xPos, yPos);
  display.setCursor(20, 0);

  // Print the text
  display.println(text);
  display.display();
}

  /*void draw_static_menu(){
    display.clear();
    display.setScale(1);
    
    display.setCursorXY(0, 0);
    display.print(F("X #: XX"));//volley ...  (1-3)
    display.setCursorXY(0, 12);
    display.print(F("X A: XX"));//powerA ...  (0-10)
    display.setCursorXY(0, 24);
    display.print(F("X B: XX"));//powerB ...  (0-10)

    //display.setCursorXY(0, 40);
    //display.print(F("DLS: XXXXX"));//DLS
    display.setCursorXY(0, 52);
    display.print(F("D: XXXX"));//DLT

    display.setScale(2);
    display.setCursorXY(70, 48);
    //display.setCursorXY(96, 52);//for text size 1
    display.print(F("X.XXv"));
    
    display.update();
  }*/


  /*void update_arrow(){//TODO option was moved to main class
    display.setScale(1);
    display.clear(0, 0, 8, 32);

    if (option == OPTION_VOLLEY){
      display.setCursorXY(0, 0);display.print(F(">"));
    }else if(option == OPTION_POWER_A){
      display.setCursorXY(0, 12);display.print(F(">"));
    }else if(option == OPTION_POWER_B){
      display.setCursorXY(0, 24);display.print(F(">"));
    }
    display.update();
  }//update_arrow */

  /*void draw_volley(uint8_t volley){
    display.setScale(1);
    display.clear(80, 0, 16, 8);
    display.setCursorXY(24, 0);
    display.print(volley);

    Serial.println(volley);

  }*/

  /*void update(){
    display.update();
  }*/

  private:
  
};

