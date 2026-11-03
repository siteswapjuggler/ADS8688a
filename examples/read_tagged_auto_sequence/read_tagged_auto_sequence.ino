/*
  Read all eight channels using SDO channel metadata.

  The ADS8688 conversion result is pipelined.  Channel tags let the host
  verify that each result belongs to the expected channel instead of relying
  only on command count.  A complete scan is published only when CH0 through
  CH7 arrive in order.  AUTO_RST is used once at startup and again only if a
  sequence error is detected.

  Hardware SPI pins vary by board.  Connect the ADS8688 SCLK, SDI, and SDO
  pins to the board hardware SPI bus and CS to the pin selected below.
*/

#include <ADS8688.h>

constexpr uint8_t CHIP_SELECT_PIN = 10;
constexpr uint8_t CHANNEL_COUNT = 8;
constexpr uint8_t CHANNEL_MASK = 0xFF;
constexpr uint8_t INPUT_RANGE = R1;

ADS8688 adc(CHIP_SELECT_PIN);
uint16_t channelValues[CHANNEL_COUNT];
uint32_t sequenceErrors = 0;

bool readTaggedScan(uint16_t values[CHANNEL_COUNT]) {
  uint16_t stagedValues[CHANNEL_COUNT];

  for (uint8_t expectedChannel = 0; expectedChannel < CHANNEL_COUNT;
       expectedChannel++) {
    const ADS8688SdoData sample = adc.noOpSdoData();

    if (!sample.valid || sample.channel != expectedChannel) {
      sequenceErrors++;
      Serial.print("Sequence drift: expected CH");
      Serial.print(expectedChannel);
      Serial.print(", received ");
      if (sample.valid) {
        Serial.print("CH");
        Serial.println(sample.channel);
      } else {
        Serial.println("invalid SDO data");
      }

      // Discard the partial scan and restart the sequencer at CH0.  Valid
      // scans continue without AUTO_RST, avoiding a reset on every pass.
      adc.autoRst();
      return false;
    }

    stagedValues[expectedChannel] = sample.value;
  }

  // Do not expose a mixture of samples from different scan sequences.
  for (uint8_t channel = 0; channel < CHANNEL_COUNT; channel++) {
    values[channel] = stagedValues[channel];
  }
  return true;
}

void setup() {
  Serial.begin(115200);

  adc.begin();
  adc.setChannelSPD(CHANNEL_MASK);
  adc.setGlobalRange(INPUT_RANGE);
  adc.setSdo(SDO_FORMAT_CHANNEL);

  // Start the first automatic CH0-CH7 sequence.  Subsequent successful
  // scans naturally wrap from CH7 back to CH0.
  adc.autoRst();
}

void loop() {
  // This validates the CH0-CH7 tags, rejects partial scans, and calls
  // AUTO_RST only when invalid or out-of-order channel data indicates drift.
  if (!readTaggedScan(channelValues)) {
    return;
  }

  for (uint8_t channel = 0; channel < CHANNEL_COUNT; channel++) {
    Serial.print("CH");
    Serial.print(channel);
    Serial.print(": ");
    Serial.print(adc.I2V(channelValues[channel], INPUT_RANGE), 5);
    Serial.print(channel == CHANNEL_COUNT - 1 ? " V\n" : " V | ");
  }

  Serial.print("Sequence errors: ");
  Serial.println(sequenceErrors);
  delay(500);
}
