//2021-01-01
class Relay{

  #define ON LOW
  #define OFF HIGH
  
  public:
    Relay(){
      pinMode(RELAY_PIN, OUTPUT);
    }//end Relay

    void operate(){
      if (time_left_us > 0){
        digitalWrite(RELAY_PIN, ON);
        time_left_us--;
      }else{
        digitalWrite(RELAY_PIN, OFF);
      }
    }//end operate

    void set_us(unsigned long time_us){
      time_left_us = time_us;
    }//end add_cs
    
    private:
      bool is_enabled = false;
      volatile unsigned long time_left_us = 0;
      
};//end Relay class
