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
 *  ALARM:  NC (you can use it to a valid Interupt Pin)
 *  DAISY:  NC
 */

#include <ADS8688.h>
#define RANGE R=0

ADS8688 bank = ADS8688();                               // Instantiate ADS8688 with PIN 10 as default CS

void setup() {
  bank.setChannelRange(0,RANGE);                        // set all channel range to +- 1.25*Vref
  bank.setAlarm(true);                                  // enable alarm detection
  bank.setChannelLowThreshold(0,bank.V2I(-3.3,RANGE));  // set low  threshold to -3.3V
  bank.setChannelHighThreshold(0,bank.V2I(3.3,RANGE));  // set high threshold to 3.3V
  Serial.begin(115200);                                 // start serial communication
  }

void loop() {
  // First you need to trigger channel sampling  
  bank.manualChannel(0);                               // select channel 0 (no sampling at this point)
  uint16_t val = bank.standBy();                       // trigger sampling of channel 0 and stand by (stand by reset tripped read states)

  // Print the collected value
  Serial.print("CH0: ");             
  Serial.print(bank.I2V(val,RANGE));
  Serial.println(" V");

  // Then you can read alarm flags
  Serial.print("Alarm Overview: "); Serial.println(bank.getAlarmOverview(),BIN);
  Serial.print("Active Flags: ");   Serial.println(bank.getActiveFlags(),BIN);
  Serial.print("Tripped Flags: ");  Serial.println(bank.getTrippedFlags(),BIN);
  Serial.println();
    
  // Wait for 
  delay(500);                                        // wait for 500 milliseconds
  }
