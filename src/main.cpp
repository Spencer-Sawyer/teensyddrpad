#include <Arduino.h>
uint8_t  TX_PINS[] = {23,19,17};
uint8_t  RX_PINS[] = {22,18,16};
uint8_t MAP[3][3] = {{KEY_A,KEY_B,KEY_C},
                     {KEY_D,KEY_E,KEY_F},
                     {KEY_G,KEY_H,KEY_I}};

void setup() {
  // put your setup code here, to run once:
  // set the TX pins to output and RX for input and make sure they are on pulldown mode
  for (auto pin : TX_PINS){
    pinMode(pin, OUTPUT);
  }
  for (auto pin : RX_PINS){
    pinMode(pin, INPUT_PULLDOWN);
  }
#if defined(USB_SERIAL)
Serial.begin(115000);
#endif
}
void reset_key(u_int8_t keycode){
  #if defined(USB_SERIAL)
Serial.print(0xFF00 | (0xFF & keycode), HEX);
#endif
#if defined(USB_KEYBOARDONLY)
Nkro.reset_key(keycode);
#endif
}
void set_key(u_int8_t keycode)
{
#if defined(USB_SERIAL)
Serial.print(keycode, HEX);
#endif
#if defined(USB_KEYBOARDONLY)
Nkro.set_key(keycode);
#endif

}
void loop() {
  // put your main code here, to run repeatedly:
auto tx_iter = 0;
for (auto tx : TX_PINS){
  digitalWriteFast(tx,HIGH);
  //wait until pin is high
  delayMicroseconds(200);
  auto rx_iter = 0;
  for (auto rx : RX_PINS){
    if (digitalReadFast(rx))
    {
        set_key(MAP[rx_iter][tx_iter]);
    }
    else{
        reset_key(MAP[rx_iter][tx_iter]);
    }
    rx_iter++;
  }
  digitalWriteFast(tx,LOW);
  //wait until pin is low
  delayMicroseconds(200);
  tx_iter++;
  }
#if defined(USB_SERIAL)
Serial.println();
delay(1000);
#endif
#if defined(USB_KEYBOARDONLY)
Nkro.send_nkro_now();
#endif

}