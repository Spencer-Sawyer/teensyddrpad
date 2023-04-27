/* 
 * READ ME:
 * Be sure to define USB_HID, USB_KEYBOARDONLY, et al. and one of the options defined below

 * #define USE_KEYBOARD
 * #define USE_SERIAL
 * #define USE_JOYSTICK
 */
#include <Arduino.h>
uint8_t  RX_PIN = 18;
uint8_t  TX_PINS[] = {10,9,8, 7};
#define TX_SIZE 1 << sizeof(TX_PINS)

#define let auto
#define ms (1000)
#define s (1000000)
#define DEBOUNCE_TIME (50*ms)
#define PIN_CHANGE_STATE_TIME (10) /*microseconds*/
#ifdef USE_KEYBOARD
uint16_t MAP[9] = {KEY_A,KEY_B,KEY_C,
                   KEY_D,KEY_E,KEY_F,
                   KEY_G,KEY_H,KEY_I};
#else
uint8_t MAP[9] = {1,2,3,
                  4,5,6,
                  7,8,9};
#endif
uint32_t debounce_up[9] =  {0,0,0,
                            0,0,0,
                            0,0,0};
uint32_t debounce_down[9] = {0,0,0,
                             0,0,0,
                             0,0,0};
bool key_state[9] = {false, false, false,
                     false, false, false,
                     false, false, false};
void setup() {
  // put your setup code here, to run once:
  // set the TX pins to output and RX for input and make sure they are on pulldown mode
  for (auto pin : TX_PINS){
    pinMode(pin, OUTPUT);
  }
    pinMode(RX_PIN, INPUT_PULLDOWN);
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

for (let tx_iter = 0;tx_iter<TX_SIZE;tx_iter++){

    for(let tx_pin_num=0;tx_pin_num<sizeof(TX_PINS);tx_pin_num++){
        if(tx_iter % (2<<tx_pin_num)) digitalWriteFast(TX_PINS[tx_pin_num],HIGH);
        else digitalWriteFast(TX_PINS[tx_pin_num],LOW);


    }
  //wait until pin is high
  delayMicroseconds(PIN_CHANGE_STATE_TIME);
  auto rx_iter = 0;
  {
      auto rx = RX_PIN;
    if (digitalReadFast(rx))
    {
      if (micros() - debounce_down[rx_iter][tx_iter] > DEBOUNCE_TIME)
      {
        set_key(MAP[rx_iter][tx_iter]);
        
        if (!key_state[rx_iter][tx_iter])
         {
           debounce_up[rx_iter][tx_iter] = micros();
         }

          key_state[rx_iter][tx_iter] = true;
      }
      
    }
    else{
      if (micros() - debounce_up[rx_iter][tx_iter] > DEBOUNCE_TIME)
      {
        reset_key(MAP[rx_iter][tx_iter]);
         
        if (key_state[rx_iter][tx_iter])
        {
          debounce_down[rx_iter][tx_iter] = micros();
        }
          key_state[rx_iter][tx_iter] = false;
        
      }
      #if defined(debug_serial)
      Serial.println(debounce_down[rx_iter][tx_iter]);
      #endif

    }
    rx_iter++;
  }
  digitalWriteFast(tx,LOW);
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