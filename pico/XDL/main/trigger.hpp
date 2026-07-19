//valkor 2026-7-16
//XDL custom 3 stage trigger
//known bug: deep and shallow increments dont interact correctly.


enum TRIGGER_STATE : uint8_t {
  TRIGGER_IDLE = 0,
  TRIGGER_TOUCH_START, //1
  TRIGGER_TOUCH_ONGOING,//2
  TRIGGER_TOUCH_END,//3
  TRIGGER_SHALLOW_START,//4
  TRIGGER_SHALLOW_ONGOING,//5
  TRIGGER_SHALLOW_END,//6
  TRIGGER_SHALLOW_INCREMENT,
  TRIGGER_DEEP_START,
  TRIGGER_DEEP_ONGOING,
  TRIGGER_DEEP_END,
  TRIGGER_DEEP_INCREMENT,
};

class Trigger {
private:

  const uint8_t TOUCH_START_MS = 2;  //has to be at least 2
  const uint8_t TOUCH_END_MS = 50;   //must be down for this long to register an end
  const uint8_t TOUCH_END_MAX_MS = 150;
  const uint8_t SHALLOW_START_MS = 3;
  const uint8_t SHALLOW_END_MS = 20;          //must be down for this long to register an end
  //const uint16_t SHALLOW_INCREMENT_MS = 250;  //for example, wait 250ms then return, and repeat
  const uint8_t DEEP_START_MS = 8;
  const uint8_t DEEP_END_MS = 20;          //must be down for this long to register an end
  const uint16_t DEEP_INCREMENT_MS = 200;  //for example, wait 250ms then return, and repeat
  const uint8_t DEBOUNCE_MS = 20;       

  uint8_t pin_touch;
  uint8_t pin_shallow;
  uint8_t pin_deep;

  unsigned long ms_touch = 0;
  unsigned long ms_shallow = 0;
  unsigned long ms_deep = 0;
  unsigned long ms_debounce = 0;
  bool touch_started = false;
  bool shallow_started = false;
  bool deep_started = false;
  bool ignore_next_touch = false;
  bool ignore_next_shallow = false;
  bool ignore_next_deep = false;

public:
  Trigger(
    uint8_t pin_touch,
    uint8_t pin_shallow,
    uint8_t pin_deep)
    : pin_touch(pin_touch),
      pin_shallow(pin_shallow),
      pin_deep(pin_deep) {
    pinMode(pin_touch, INPUT_PULLUP);
    pinMode(pin_shallow, INPUT_PULLUP);
    pinMode(pin_deep, INPUT_PULLUP);
  }  //Trigger

  TRIGGER_STATE operate() {
    unsigned long ms = millis();
    if (ms < 1000) return TRIGGER_IDLE;  //trigger state invalid on power on

    bool deep = !digitalRead(pin_deep);
    if (deep == true) {
      if (ms_deep == 0) {
        ms_deep = ms;
      } else if (ms - ms_deep >= DEEP_INCREMENT_MS) {
        ms_deep = ms;
        ms_debounce = ms;
        ignore_next_deep = true;
        return TRIGGER_DEEP_INCREMENT;
      } else if (ms - ms_deep >= DEEP_START_MS) {
        ignore_next_shallow = true;
        if (deep_started == true) {
          return TRIGGER_DEEP_ONGOING;
        } else if (ms - ms_debounce >= DEBOUNCE_MS){
          deep_started = true;
          ms_debounce = ms;
          return TRIGGER_DEEP_START;
        }else{//too soon, ignored
          ms_deep = 0;
          return TRIGGER_IDLE;
        }
      }
    } else {
      if (ms - ms_deep >= DEEP_START_MS
      and ms - ms_deep >= DEBOUNCE_MS
      and deep_started == true) {  //deep end
        deep_started = false;
        ms_deep = 0;
        ms_debounce = ms;
        if (ignore_next_deep == true) {
          ignore_next_deep = false;
        } else {
          return TRIGGER_DEEP_END;
        }
      }else{
        deep_started = false;
        ms_deep = 0;
      }
    }  //deep?

    bool shallow = !digitalRead(pin_shallow);
    if (shallow == true) {
      if (ms_shallow == 0) {
        ms_shallow = ms;
      // } else if (ms - ms_shallow >= SHALLOW_INCREMENT_MS) {
      //   ms_shallow = ms;
      //   ignore_next_shallow = true;
      //   ms_debounce = ms;
      //   if (deep == false){
      //     return TRIGGER_SHALLOW_INCREMENT;
      //   }
      } else if (ms - ms_shallow >= SHALLOW_START_MS) {
        ignore_next_touch = true;
        if (shallow_started == true) {
          return TRIGGER_SHALLOW_ONGOING;
        } else if (ms - ms_debounce >= DEBOUNCE_MS){
          shallow_started = true;
          ms_debounce = ms;
          return TRIGGER_SHALLOW_START;
        }else{//too soon, ignored
          ms_shallow = 0;
          return TRIGGER_IDLE;
        }
      }
    } else {
      if (ms - ms_shallow >= SHALLOW_END_MS
      and ms - ms_debounce >= DEBOUNCE_MS
      and shallow_started == true) {  //shallow end
        shallow_started = false;
        ms_shallow = 0;
        ms_debounce = ms;
        if (ignore_next_shallow == true) {
          ignore_next_shallow = false;
        } else {
          return TRIGGER_SHALLOW_END;
        }
      }else{
        shallow_started = false;
        ms_shallow = 0;
      }
    }  //shallow?

    bool touch = digitalRead(pin_touch);
    if (touch == true) {
      if (ms_touch == 0) {
        ms_touch = ms;
      } else if (ms - ms_touch >= TOUCH_START_MS) {
        if (touch_started == true) {
          return TRIGGER_TOUCH_ONGOING;
        } else if(ms - ms_debounce >= DEBOUNCE_MS){
          touch_started = true;
          ms_debounce = ms;
          return TRIGGER_TOUCH_START;
        }else{//too soon, ignored
          ms_touch = 0;
          return TRIGGER_IDLE;
        }
      }
    } else {
      if (ms - ms_touch >= TOUCH_START_MS
      and ms - ms_debounce >= DEBOUNCE_MS
      and ms - ms_touch <= TOUCH_END_MAX_MS
      and touch_started == true) {  //touching end
        touch_started = false;
        ms_touch = 0;
        ms_debounce = ms;
        ignore_next_touch = false;
        return TRIGGER_TOUCH_END;
      }else{
        touch_started = false;
        ms_touch = 0;
      }
    }  //touching?

    return TRIGGER_IDLE;
  }  //operate

};  //Trigger