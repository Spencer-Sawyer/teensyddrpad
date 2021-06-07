//#define USB_HID
#include <Arduino.h>
uint8_t  RX_PINS[] = {23,21,19};
uint8_t  TX_PINS[] = {22,20,18};
/*
#if (defined(USB_KEYBOARDONLY) || defined(USB_HID) \
 ||  defined(USB_SERIAL_HID) || defined(USB_TOUCHSCREEN) \
 ||  defined(USB_HID_TOUCHSCREEN) || defined(USB_EVERYTHING)) \
 && 0
#define USE_KEYBOARD
#elif 0
#define USE_SERIAL
#elif 1
#define USE_JOYSTICK
#endif*/
//#define debug_serial
#define ms (1000)
#define s (1000000)
#define DEBOUNCE_TIME (50*ms)
#define PIN_CHANGE_STATE_TIME (10) /*microseconds*/
#ifdef USE_KEYBOARD
uint16_t MAP[3][3] = {{KEY_A,KEY_B,KEY_C},
                      {KEY_D,KEY_E,KEY_F},
                      {KEY_G,KEY_H,KEY_I}};
#else
uint8_t MAP[3][3] = {{4,7,1},
                     {2,9,5},
                     {8,6,3}};
#endif
uint32_t debounce_up[3][3] = {{0,0,0},
                              {0,0,0},
                              {0,0,0}};
uint32_t debounce_down[3][3] = {{0,0,0},
                                {0,0,0},
                                {0,0,0}};
bool keystate[3][3] = {{0,0,0},
                       {0,0,0},
                       {0,0,0}};
void setup() {
  // put your setup code here, to run once:
  // set the TX pins to output and RX for input and make sure they are on pulldown mode
  for (auto pin : TX_PINS){
    pinMode(pin, OUTPUT);
  }
  for (auto pin : RX_PINS){
    pinMode(pin, INPUT_PULLDOWN);
  }
#if defined(USB_SERIAL) || defined(debug_serial)
Serial.begin(115000);
#endif
#if defined(USE_JOYSTICK)
Joystick.begin();
Joystick.useManualSend(true);
#endif
}
void reset_key(u_int8_t keycode){
#if defined(USB_SERIAL)
Serial.print(0xFF00 | (0xFF & keycode), HEX);
#endif
#if defined(USE_KEYBOARD)
Nkro.reset_key(keycode);
#endif
#if defined(USE_JOYSTICK)
Joystick.button(keycode,false);
#endif

}
void set_key(u_int8_t keycode)
{
#if defined(USB_SERIAL)
Serial.print(keycode, HEX);
#endif
#if defined(USE_KEYBOARD)
Nkro.set_key(keycode);
#endif
#if defined(USE_JOYSTICK)
Joystick.button(keycode,true);
#endif

}
void loop() {
  // put your main code here, to run repeatedly:
auto tx_iter = 0;
for (auto tx : TX_PINS){

  digitalWriteFast(tx,HIGH);
  //wait until pin is high
  delayMicroseconds(PIN_CHANGE_STATE_TIME);
  auto rx_iter = 0;
  for (auto rx : RX_PINS){
    if (digitalReadFast(rx))
    {
      if (micros() - debounce_down[rx_iter][tx_iter] > DEBOUNCE_TIME)
      {
        set_key(MAP[rx_iter][tx_iter]);
        
        if (!keystate[rx_iter][tx_iter])
         {
           debounce_up[rx_iter][tx_iter] = micros();
         }

        keystate[rx_iter][tx_iter] = true;
      }
      
    }
    else{
      if (micros() - debounce_up[rx_iter][tx_iter] > DEBOUNCE_TIME)
      {
        reset_key(MAP[rx_iter][tx_iter]);
         
        if (keystate[rx_iter][tx_iter]) 
        {
          debounce_down[rx_iter][tx_iter] = micros();
        }
        keystate[rx_iter][tx_iter] = false;
        
      }
      #if defined(debug_serial)
      Serial.println(debounce_down[rx_iter][tx_iter]);
      #endif

    }
    rx_iter++;
  }
  digitalWriteFast(tx,LOW);
  tx_iter++;
  }
#if defined(USB_SERIAL)
Serial.println();
delay(1000);
#endif
#if defined(USE_KEYBOARD)
Nkro.send_nkro_now();
#endif
#if defined(USE_JOYSTICK)
Joystick.send_now();
#endif

}