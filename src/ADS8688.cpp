// ADS8688 library for Arduino
// initial release by Sylvain GARNAVAULT - 2016/03/13

#include "ADS8688.h"

static uint8_t manualChannelCommand(uint8_t ch) {
    switch (ch) {
        case 0:  return MAN_Ch_0;
        case 1:  return MAN_Ch_1;
        case 2:  return MAN_Ch_2;
        case 3:  return MAN_Ch_3;
        case 4:  return MAN_Ch_4;
        case 5:  return MAN_Ch_5;
        case 6:  return MAN_Ch_6;
        case 7:  return MAN_Ch_7;
        case 8:  return MAN_AUX;
        default: return MAN_Ch_0;
    }
}

////////////////////////
//  INSTANCIATION     //
////////////////////////

ADS8688::ADS8688() {
    _cs = 10;                     // default chip select pin
    _mode = MODE_IDLE;            // start in Idle mode
    _vref = 4.096;                // vref at 4.096
    _feature = 0;                 // start with no feature
    pinMode(_cs,OUTPUT);          // set the pin as output
    digitalWrite(_cs,HIGH);       // set the pin to default HIGH state
    SPI.begin();                  // initiate SPI
    }

ADS8688::ADS8688(byte cs) {
	_cs = cs;                     // choose the chip select pin
    _mode = MODE_IDLE;            // start in Idle mode
    _vref = 4.096;                // vref at 4.096
    _feature = 0;                 // start with no feature
	pinMode(_cs,OUTPUT);          // set the pin as output
    digitalWrite(_cs,HIGH);       // set the pin to default HIGH state
    SPI.begin();                  // initiate SPI
    }

/////////////////////////
//  PUBLIC METHODS    //
/////////////////////////

uint16_t ADS8688::noOp() {
    return cmdRegister(NO_OP);
    }

uint16_t ADS8688::standBy() {
    return cmdRegister(STDBY);
    }

uint16_t ADS8688::powerDown() {
    return cmdRegister(PWR_DN);
    }

uint16_t ADS8688::reset() {
    uint16_t result = cmdRegister(RST);
    _feature = 0;
    return result;
    }

uint16_t ADS8688::autoRst() {
    return cmdRegister(AUTO_RST);
    }

uint16_t ADS8688::manualChannel(uint8_t ch) {
    return cmdRegister(manualChannelCommand(ch));
    }

ADS8688SdoData ADS8688::noOpSdoData() {
    return cmdRegisterSdoData(NO_OP);
    }

ADS8688SdoData ADS8688::autoRstSdoData() {
    return cmdRegisterSdoData(AUTO_RST);
    }

ADS8688SdoData ADS8688::manualChannelSdoData(uint8_t ch) {
    return cmdRegisterSdoData(manualChannelCommand(ch));
    }

//-----------------------------------------------------------------------------

void ADS8688::setChannelSPD(uint8_t flag) {
    setChannelSequence(flag);
    setChannelPowerDown((uint8_t)~flag);
    }

void ADS8688::setChannelSequence(uint8_t flag) {
    writeRegister(AUTO_SEQ_EN,flag);
    }

void ADS8688::setChannelPowerDown(uint8_t flag) {
    writeRegister(CH_PWR_DN,flag);
    }

uint8_t ADS8688::getChannelSequence() {
    return readRegister(AUTO_SEQ_EN);
    }

uint8_t ADS8688::getChannelPowerDown() {
    return readRegister(CH_PWR_DN);
    }

//-----------------------------------------------------------------------------

uint8_t ADS8688::getChannelRange(uint8_t ch) {
    uint8_t reg;
    switch (ch) {
        case 0:  reg = RG_Ch_0;break;
        case 1:  reg = RG_Ch_1;break;
        case 2:  reg = RG_Ch_2;break;
        case 3:  reg = RG_Ch_3;break;
        case 4:  reg = RG_Ch_4;break;
        case 5:  reg = RG_Ch_5;break;
        case 6:  reg = RG_Ch_6;break;
        case 7:  reg = RG_Ch_7;break;
        default: reg = RG_Ch_0;break;
        }
    return readRegister(reg);
    }

void ADS8688::setChannelRange(uint8_t ch, uint8_t range) {
    uint8_t reg;
    switch (ch) {
        case 0:  reg = RG_Ch_0;break;
        case 1:  reg = RG_Ch_1;break;
        case 2:  reg = RG_Ch_2;break;
        case 3:  reg = RG_Ch_3;break;
        case 4:  reg = RG_Ch_4;break;
        case 5:  reg = RG_Ch_5;break;
        case 6:  reg = RG_Ch_6;break;
        case 7:  reg = RG_Ch_7;break;
        default: reg = RG_Ch_0;break;
        }
    writeRegister(reg,range);
    }

void ADS8688::setGlobalRange(uint8_t range) {
    for (uint8_t i=0;i<8;i++) setChannelRange(i,range);
    }

//-----------------------------------------------------------------------------

uint8_t ADS8688::getId() {
    return (getFeatureSelect() >> 6);
    }

void ADS8688::setId(uint8_t id) {
    getFeatureSelect();
    _feature = (_feature & 0b00010111) | ((id & 0b11)<<6);
    writeRegister(FT_SEL,_feature);
    }

bool ADS8688::getAlarm() {
    return (getFeatureSelect() >> 4) & 1;
    }

void ADS8688::setAlarm(bool alarm) {
    getFeatureSelect();
    _feature = (_feature & 0b11000111) | ((alarm == true)<<4);
    writeRegister(FT_SEL,_feature);
    }

uint8_t ADS8688::getSdo() {
    return (getFeatureSelect() & 0b111);
    }

void ADS8688::setSdo(uint8_t sdo) {
    if (sdo > SDO_FORMAT_CHANNEL_DEVICE_RANGE) sdo = SDO_FORMAT_DATA_ONLY;
    getFeatureSelect();
    _feature = (_feature & 0b11010000) | sdo;
    writeRegister(FT_SEL,_feature);
    }

uint8_t ADS8688::getFeatureSelect() {
    _feature = readRegister(FT_SEL);
    return _feature;
    }

void ADS8688::setFeatureSelect(uint8_t id, bool alarm, uint8_t sdo) {
    if (sdo > SDO_FORMAT_CHANNEL_DEVICE_RANGE) sdo = SDO_FORMAT_DATA_ONLY;
    _feature = ((id & 0b11)<<6) | ((alarm == true)<<4) | sdo;
    writeRegister(FT_SEL,_feature);
    }

//-----------------------------------------------------------------------------

uint8_t ADS8688::getAlarmOverview() {
    return readRegister(ALARM_OVERVIEW);
    }

uint8_t ADS8688::getFirstTrippedFlag() {
    return readRegister(ALARM_CH0_TRIPPED_FLAG);
    }

uint8_t ADS8688::getSecondTrippedFlag() {
    return readRegister(ALARM_CH4_TRIPPED_FLAG);
    }

uint16_t ADS8688::getTrippedFlags() {
    uint8_t MSB = readRegister(ALARM_CH0_TRIPPED_FLAG);
    uint8_t LSB = readRegister(ALARM_CH4_TRIPPED_FLAG);
    return (MSB << 8) | LSB;
    }

uint8_t ADS8688::getFirstActiveFlag() {
    return readRegister(ALARM_CH0_ACTIVE_FLAG);
    }

uint8_t ADS8688::getSecondActiveFlag() {
    return readRegister(ALARM_CH4_ACTIVE_FLAG);
    }

uint16_t ADS8688::getActiveFlags() {
    uint8_t MSB = readRegister(ALARM_CH0_ACTIVE_FLAG);
    uint8_t LSB = readRegister(ALARM_CH4_ACTIVE_FLAG);
    return (MSB << 8) | LSB;
    }

//-----------------------------------------------------------------------------

uint8_t ADS8688::getChannelHysteresis(uint8_t ch) {
    uint8_t reg = 5*(ch>7?7:ch) + CH0_HYST;
    return readRegister(reg);
    }

uint16_t ADS8688::getChannelLowThreshold(uint8_t ch) {
    uint8_t reg = 5*(ch>7?7:ch) + CH0_LT_MSB;
    uint8_t MSB = readRegister(reg);
    uint8_t LSB = readRegister(reg+1);
    return (MSB << 8) | LSB;
    }

uint16_t ADS8688::getChannelHighThreshold(uint8_t ch) {
    uint8_t reg = 5*(ch>7?7:ch) + CH0_HT_MSB;
    uint8_t MSB = readRegister(reg);
    uint8_t LSB = readRegister(reg+1);
    return (MSB << 8) | LSB;
    }

void ADS8688::setChannelHysteresis(uint8_t ch, uint8_t val) {
    uint8_t reg = 5*(ch>7?7:ch) + CH0_HYST;
    writeRegister(reg,val);
    }

void ADS8688::setChannelLowThreshold(uint8_t ch, uint16_t val) {
    uint8_t reg = 5*(ch>7?7:ch) + CH0_LT_MSB;
    writeRegister(reg,val>>8);
    writeRegister(reg+1,val&255);
    }

void ADS8688::setChannelHighThreshold(uint8_t ch, uint16_t val)  {
    uint8_t reg = 5*(ch>7?7:ch) + CH0_HT_MSB;
    writeRegister(reg,val>>8);
    writeRegister(reg+1,val&255);
    }

//-----------------------------------------------------------------------------

uint8_t ADS8688::getCommandReadBack() {
    return readRegister(CMD_READBACK);
    }

//-----------------------------------------------------------------------------

void ADS8688::setVREF(float vref) {
    _vref = vref;
    }

float ADS8688::I2V(uint16_t x, uint8_t range) {
    float out_min, out_max;
    switch (range) {
        case R1:
            out_min = -1.25 * _vref;
            out_max = 1.25 * _vref;
            break;
        case R2:
            out_min = -0.625 * _vref;
            out_max = 0.625 * _vref;
            break;
        case R3:
            out_min = -0.3125 * _vref;
            out_max = 0.3125 * _vref;
            break;
        case R4:
            out_min = -0.15625 * _vref;
            out_max = 0.15625 * _vref;
            break;
        case R5:
            out_min = 0 * _vref;
            out_max = 2.5 * _vref;
            break;
        case R6:
            out_min = 0 * _vref;
            out_max = 1.25 * _vref;
            break;
        case R7:
            out_min = 0 * _vref;
            out_max = 0.625 * _vref;
            break;
        case R8:
            out_min = 0 * _vref;
            out_max = 0.3125 * _vref;
            break;
        default:
            out_min = -2.5 * _vref;
            out_max = 2.5 * _vref;
            break;
            
    }
    return (float)x * (out_max - out_min) / 65536.0f + out_min;
    }

uint16_t ADS8688::V2I(float x, uint8_t range) {
    float in_min, in_max;
    switch (range) {
        case R1:
            in_min = -1.25 * _vref;
            in_max = 1.25 * _vref;
            break;
        case R2:
            in_min = -0.625 * _vref;
            in_max = 0.625 * _vref;
            break;
        case R3:
            in_min = -0.3125 * _vref;
            in_max = 0.3125 * _vref;
            break;
        case R4:
            in_min = -0.15625 * _vref;
            in_max = 0.15625 * _vref;
            break;
        case R5:
            in_min = 0 * _vref;
            in_max = 2.5 * _vref;
            break;
        case R6:
            in_min = 0 * _vref;
            in_max = 1.25 * _vref;
            break;
        case R7:
            in_min = 0 * _vref;
            in_max = 0.625 * _vref;
            break;
        case R8:
            in_min = 0 * _vref;
            in_max = 0.3125 * _vref;
            break;
        default:
            in_min = -2.5 * _vref;
            in_max = 2.5 * _vref;
            break;
            
    }
    if (x <= in_min) return 0;
    if (x >= in_max) return 0xFFFF;
    return (uint16_t)((x - in_min) * 65536.0f / (in_max - in_min));
}

/////////////////////////
//  PRIVATE METHODS    //
/////////////////////////

void ADS8688::writeRegister(uint8_t reg, uint8_t val) {
    SPI.beginTransaction(SPISettings(17000000, MSBFIRST, SPI_MODE1));
    digitalWrite(_cs, LOW);
    SPI.transfer((reg << 1) | 0x01);
    SPI.transfer(val);;
    SPI.transfer(0x00);
    digitalWrite(_cs, HIGH);
    SPI.endTransaction();
    // Program-register access does not exit RESET, STDBY, or PWR_DN.
    if (_mode == MODE_IDLE || _mode >= MODE_PROG) _mode = MODE_PROG;
    }

uint8_t ADS8688::readRegister(uint8_t reg) {
    SPI.beginTransaction(SPISettings(17000000, MSBFIRST, SPI_MODE1));
    digitalWrite(_cs, LOW);
    SPI.transfer((reg << 1) | 0x00);
    SPI.transfer(0x00);
    byte result = SPI.transfer(0x00);
    digitalWrite(_cs, HIGH);
    SPI.endTransaction();
    // Program-register access does not exit RESET, STDBY, or PWR_DN.
    if (_mode == MODE_IDLE || _mode >= MODE_PROG) _mode = MODE_PROG;
    return result;
    }

uint16_t ADS8688::cmdRegister(uint8_t reg) {
    return cmdRegisterTransfer(reg, NULL);
    }

ADS8688SdoData ADS8688::cmdRegisterSdoData(uint8_t reg) {
    ADS8688SdoData sdoData;
    sdoData.value = 0;
    sdoData.channel = 0xFF;
    sdoData.device = 0xFF;
    sdoData.range = 0xFF;
    sdoData.sdoFormat = _feature & 0b011;
    sdoData.valid = false;
    cmdRegisterTransfer(reg, &sdoData);
    return sdoData;
    }

uint16_t ADS8688::cmdRegisterTransfer(uint8_t reg, ADS8688SdoData *sdoData) {
    SPI.beginTransaction(SPISettings(17000000, MSBFIRST, SPI_MODE1));
    digitalWrite(_cs, LOW);
    SPI.transfer(reg);
    SPI.transfer(0x00);
    uint16_t result = 0;
    const bool hasConversionResult = _mode > MODE_PROG;
    const bool isManualChannelCommand =
        reg >= MAN_Ch_0 && reg <= MAN_AUX && (reg & 0x03) == 0;
    const bool needsCompleteFrame =
        reg == AUTO_RST || isManualChannelCommand;

    // AUTO_RST and MAN_Ch_n need a complete 32-SCLK command frame to
    // accurately acquire the first selected channel in the next frame.
    if (hasConversionResult || needsCompleteFrame) {
        byte MSB = SPI.transfer(0x00);
        byte LSB = SPI.transfer(0x00);
        if (hasConversionResult) {
            result = ((uint16_t)MSB << 8) | LSB;
            if (sdoData != NULL) {
                byte metadataHigh = SPI.transfer(0x00);
                byte metadataLow = SPI.transfer(0x00);
                sdoData->value = result;
                sdoData->valid = true;
                if (sdoData->sdoFormat >= SDO_FORMAT_CHANNEL) {
                    sdoData->channel = metadataHigh >> 4;
                    }
                if (sdoData->sdoFormat >= SDO_FORMAT_CHANNEL_DEVICE) {
                    sdoData->device = (metadataHigh >> 2) & 0x03;
                    }
                if (sdoData->sdoFormat >= SDO_FORMAT_CHANNEL_DEVICE_RANGE) {
                    sdoData->range = ((metadataHigh & 0x03) << 1) | (metadataLow >> 7);
                    }
                }
            }
        }
    digitalWrite(_cs, HIGH);
    SPI.endTransaction();
    
    // Keep CS high for the required wake-up time after a conversion command.
    if (_mode == MODE_STANDBY && needsCompleteFrame) delayMicroseconds(20);

    // when exit power down it takes 15 ms to be operationnal
    if (_mode == MODE_POWER_DN) delay(15);
    
    switch (reg) {
        case NO_OP:
            switch (_mode) {
                case MODE_PROG :    _mode = MODE_IDLE;
                    break;
                case MODE_AUTO_RST: _mode = MODE_AUTO;
                    break;
            }
            break;
        case STDBY:
            _mode = MODE_STANDBY;
            break;
        case PWR_DN:
            _mode = MODE_POWER_DN;
            break;
        case RST:
            _mode = MODE_RESET;
            break;
        case AUTO_RST:
            _mode = MODE_AUTO_RST;
            break;
        default:
            _mode = MODE_MANUAL;
            break;
    }
    return result;
    }

