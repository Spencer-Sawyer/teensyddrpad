#include <Arduino.h>
uint8_t  TX_PINS[] = {22,19,17};
uint8_t  RX_PINS[] = {21,18,16};
uint16_t active_button_bitmap = 0x0;

void setup() {
  // put your setup code here, to run once:
  // set the TX pins to output and RX for input and make sure they are on pulldown mode
  for (auto pin : TX_PINS){
    pinMode(pin, OUTPUT);
  }
  for (auto pin : RX_PINS){
    pinMode(pin, INPUT_PULLDOWN);
  }
  #ifndef USB_KEYBOARD
Serial.begin(115000);
#endif
  

}

void loop() {
  // put your main code here, to run repeatedly:
}