// ADS8688 library for Arduino
// initial release by Sylvain GARNAVAULT - 2016/03/13

#include "ADS8688.h"

////////////////////////
//  INSTANCIATION     //
////////////////////////

/*
	mettre un vrai channle, un ADS.begin et un setSPIChannel, un setSPISpeed aussi.
	vérifier les fonctionnements des constantes suivantes sur le TC275

    //list of SPI channel available on TC275----------------------------------------------------------------------

    //#define SPI_channel BOARD_SPI_SS0        //        CS = 10  MISO = P201.1, MOSI = P201.4 SCK = P201.3
    //#define SPI_channel BOARD_SPI_SS0_S1     //(210u)  CS = 10  MISO = p12,    MOSI = p11    SCK = p13
    //#define SPI_channel BOARD_SOFT_SPI_SS0   //(211u)  CS = 10  MISO = p50,    MOSI = p51    SCK = p52
    //#define SPI_channel BOARD_SOFT_SPI_SS2   //(153u)  CS = 53  MISO = p50,    MOSI = p51    SCK = p52
    //#define SPI_channel BOARD_SPI_SS1        //        MISO = P201.1, MOSI = P201.4 SCK = P201.3
    #define SPI_channel BOARD_SPI_SS2          // Not used yet 
*/

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
	#ifdef __TC27XX__
	SPI.begin(_cs);           	  // initiate SPI
	#else
	SPI.begin();                  // initiate SPI
	#endif
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
    return cmdRegister(RST);
}

uint16_t ADS8688::autoRst() {
    return cmdRegister(AUTO_RST);
}

uint16_t ADS8688::manualChannel(uint8_t ch) {
    uint8_t reg;
    switch (ch) {
        case 0:  reg = MAN_Ch_0;break;
        case 1:  reg = MAN_Ch_1;break;
        case 2:  reg = MAN_Ch_2;break;
        case 3:  reg = MAN_Ch_3;break;
        case 4:  reg = MAN_Ch_4;break;
        case 5:  reg = MAN_Ch_5;break;
        case 6:  reg = MAN_Ch_6;break;
        case 7:  reg = MAN_Ch_7;break;
        case 8:  reg = MAN_AUX; break;
        default: reg = MAN_Ch_0;break;
    }
    return cmdRegister(reg);
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
    _feature = (_feature & 0b00010111) | ((id & 0b11)<<6);
    writeRegister(FT_SEL,_feature);
}

bool ADS8688::getAlarm() {
    return (getFeatureSelect() >> 4) & 1;
}

void ADS8688::setAlarm(bool alarm) {
    _feature = (_feature & 0b11000111) | ((alarm == true)<<4);
    writeRegister(FT_SEL,_feature);
}

uint8_t ADS8688::getSdo() {
    return (getFeatureSelect() & 0b111);
}

void ADS8688::setSdo(uint8_t sdo) {
    _feature = (_feature & 0b11010000) | (sdo & 0b111);
    writeRegister(FT_SEL,_feature);
}

uint8_t ADS8688::getFeatureSelect() {
    return readRegister(FT_SEL);
}

void ADS8688::setFeatureSelect(uint8_t id, bool alarm, uint8_t sdo) {
    _feature = ((id & 0b11)<<6) | ((alarm == true)<<4) | (sdo & 0b111);
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
    return (float)x * (out_max - out_min) / 65535. + out_min;
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
    return (x-in_min) * 65535. / (in_max-in_min);
}

/////////////////////////
//  PRIVATE METHODS    //
/////////////////////////

void ADS8688::writeRegister(uint8_t reg, uint8_t val) {
    SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1));
    digitalWrite(_cs, LOW);
	#ifdef __TC27XX__
	SPI.transfer(_ch,(reg << 1) | 0x01,SPI_CONTINUE);
	SPI.transfer(_ch,val,SPI_CONTINUE);
	SPI.transfer(_ch,0x00,SPI_LAST);
	#else
	SPI.transfer((reg << 1) | 0x01);
	SPI.transfer(val);
	SPI.transfer(0x00);
	#endif
    digitalWrite(_cs, HIGH);
    SPI.endTransaction();
    _mode = MODE_PROG;
}

uint8_t ADS8688::readRegister(uint8_t reg) {
    SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1));
    digitalWrite(_cs, LOW);
	#ifdef __TC27XX__
	SPI.transfer(_cs,(reg << 1) | 0x00,SPI_CONTINUE);
	SPI.transfer(_cs,0x00,SPI_CONTINUE);
	byte result = SPI.transfer(_cs,0x00,SPI_LAST);
	#else
	SPI.transfer((reg << 1) | 0x00);
	SPI.transfer(0x00);
	byte result = SPI.transfer(0x00);
	#endif
    digitalWrite(_cs, HIGH);
    SPI.endTransaction();
    _mode = MODE_PROG;
    return result;
}

uint16_t ADS8688::cmdRegister(uint8_t reg) {
	int16_t result = 0;
    SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1));
    digitalWrite(_cs, LOW); 
    
	#ifdef __TC27XX__	
    if (_mode > 4) {
		SPI.transfer(_cs,reg,SPI_CONTINUE);
		SPI.transfer(_cs,0x00,SPI_CONTINUE);
		byte MSB = SPI.transfer(_cs,0x00,SPI_CONTINUE);
		byte LSB = SPI.transfer(_cs,0x00,SPI_LAST);
		result = ( MSB << 8) | LSB;
	}
	else {
		// only 16 bit if POWERDOWN or STDBY or RST or IDLE
		SPI.transfer(_cs,reg,SPI_CONTINUE);
		SPI.transfer(_cs,0x00,SPI_LAST);      
	}       
	#else
	SPI.transfer(reg);
	SPI.transfer(0x00);
	if (_mode > 4) {
		// only 16 bit if POWERDOWN or STDBY or RST or IDLE
		byte MSB = SPI.transfer(0x00);
		byte LSB = SPI.transfer(0x00);
		result = ( MSB << 8) | LSB;
	}
	#endif
	
    digitalWrite(_cs, HIGH);
    SPI.endTransaction();
    
    // when exit power down it takes 15 ms to be operationnal
    if (_mode == MODE_POWER_DN) delay(15);
    
    switch (reg) {
        case NO_OP:
            switch (_mode) {
                case MODE_RESET:    _mode = MODE_IDLE;
                    break;
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