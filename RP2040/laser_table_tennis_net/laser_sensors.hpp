/* for IR receivers
 *  receiver+ to G
 * receiver- to analog pin
 * A to resistor to +
 * 
 * for normal receivers, there is no polarity
 */

#define READS_PER_CYCLE 1
const uint8_t PINS[] = {A0}; // bottom -> top
//const uint8_t PINS[] = {A0, A4, A1, A2, A3}; // bottom -> top
#define NUM_SENSORS sizeof(PINS)

//statuses
#define CALIBRATING 0
#define READY 1
#define TRIGGERED 2
#define RECOVERING 3

class LaserSensors{ public:

  

  LaserSensors(){
    //TODO constructor
  }

  uint8_t operate(){
    for (int sensor=0; sensor<NUM_SENSORS; sensor++){
      int16_t v = analogRead(PINS[sensor]);
      if (v >= 100){
        return TRIGGERED;
      }
    }
    return 0;
  }//end operate()

  uint8_t status = CALIBRATING;
  //TODO trigger threshhold array

};//end class ProximitySensor
