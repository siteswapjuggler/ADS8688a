/*  ADS8688 library example
 *   
 *  PIN CONNECTION: 
 *  
 *  REFSEL: GND (enable internal 4.096V reference)
 *  RST/PD: +5V/+3.3V
 *  AVDD:   +5V/+3.3V
 *  DVDD:   +5V/+3.3V
 *  CS:     pin 10 / MEGA pin 10
 *  MOSI:   pin 11 / MEGA pin 51
 *  MISO:   pin 12 / MEGA pin 50
 *  SCK:    pin 13 / MEGA pin 52
 *  REFIO:  NC
 *  ALARM:  NC
 *  DAISY:  NC
 */

#include <ADS8688.h>

ADS8688 bank = ADS8688();               // Instantiate ADS8688 with PIN 10 as default CS

void setup() {
  bank.setChannelSPD(0b11111111);
  bank.setGlobalRange(R1);              // set range for all channels
  bank.autoRst();                       // reset auto sequence
  Serial.begin(115200);                 // start serial communication
  }

void loop() {
  Serial.print("values: ");             // print label
  for (byte i=0;i<8;i++) {
    uint16_t val = bank.noOp();         // trigger samples
    Serial.print(bank.I2V(val,R1));     // print value in Volts
    Serial.println(i!=7?" V | ":" V");  // print Volt label
  }
  delay(500);                           // wait for 500 milliseconds
  }
