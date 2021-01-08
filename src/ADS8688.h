// ADS8688 library for Arduino
// initial release by Sylvain GARNAVAULT - 2016/03/13

#ifndef ADS8688_h

    #define ADS8688_h
    #include "Arduino.h"
    #include "SPI.h"

    //COMMAND REGISTER MAP --------------------------------------------------------------------------------------------

    #define NO_OP     0x00  // Continue operation in previous mode
    #define STDBY     0x82  // Device is placed into standby mode
    #define PWR_DN    0x83  // Device is powered down
    #define RST       0x85  // Program register is reset to default
    #define AUTO_RST  0xA0  // Auto mode enabled following a reset
    #define MAN_Ch_0  0xC0  // Channel 0 input is selected
    #define MAN_Ch_1  0xC4  // Channel 1 input is selected
    #define MAN_Ch_2  0xC8  // Channel 2 input is selected
    #define MAN_Ch_3  0xCC  // Channel 3 input is selected
    #define MAN_Ch_4  0xD0  // Channel 4 input is selected
    #define MAN_Ch_5  0xD4  // Channel 5 input is selected
    #define MAN_Ch_6  0xD8  // Channel 6 input is selected
    #define MAN_Ch_7  0xDC  // Channel 7 input is selected
    #define MAN_AUX   0xE0  // AUX channel input is selected

    // PROGRAM REGISTER MAP -------------------------------------------------------------------------------------------

    // AUTO SCAN SEQUENCING CONTROL
    #define AUTO_SEQ_EN   0x01  // Auto Squencing Enable: default 0xFF - bitX to enable chX
    #define CH_PWR_DN     0x02  // Channel Power Down: default 0x00 - bitX to power down chX

    // DEVICE FEATURES SELECTION CONTROL
    #define FT_SEL        0x03  // Feature Select: default 0x00
                                // bit 7-6 for daisy chain ID, bit 4 for ALARM feature, bit 2-0 SDO data format bits

    // RANGE SELECT REGISTERS
    #define RG_Ch_0       0x05   // Channel 0 Input Range: default 0x00 - bit 3-0 to select range
    #define RG_Ch_1       0x06   // Channel 1 Input Range: default 0x00 - bit 3-0 to select range
    #define RG_Ch_2       0x07   // Channel 2 Input Range: default 0x00 - bit 3-0 to select range
    #define RG_Ch_3       0x08   // Channel 3 Input Range: default 0x00 - bit 3-0 to select range
    #define RG_Ch_4       0x09   // Channel 4 Input Range: default 0x00 - bit 3-0 to select range
    #define RG_Ch_5       0x0A   // Channel 5 Input Range: default 0x00 - bit 3-0 to select range
    #define RG_Ch_6       0x0B   // Channel 6 Input Range: default 0x00 - bit 3-0 to select range
    #define RG_Ch_7       0x0C   // Channel 7 Input Range: default 0x00 - bit 3-0 to select range

    // ALARM FLAG REGISTERS (Read-only)
    #define ALARM_OVERVIEW          0x10 // ALARM Overview Tripped Flag
    #define ALARM_CH0_TRIPPED_FLAG  0x11 // ALARM Ch 0-3 Tripped-Flag
    #define ALARM_CH0_ACTIVE_FLAG   0x12 // ALARM Ch 0-3 Active-Flag
    #define ALARM_CH4_TRIPPED_FLAG  0x13 // ALARM Ch 4-7 Tripped-Flag
    #define ALARM_CH4_ACTIVE_FLAG   0x14 // ALARM Ch 4-7 Active-Flag

    // ALARM THRESHOLD REGISTERS
    #define CH0_HYST      0x15   // Ch 0 Hysteresis
    #define CH0_HT_MSB    0x16   // Ch 0 High Threshold MSB
    #define CH0_HT_LSB    0x17   // Ch 0 High Threshold LSB
    #define CH0_LT_MSB    0x18   // Ch 0 Low Threshold MSB
    #define CH0_LT_LSB    0x19   // Ch 0 Low Threshold LSB
    //... CHx register address are Ch0 + 5x

    // COMMAND READ BACK (Read-Only)
    #define CMD_READBACK  0x3F   // Command Read Back

    // SPECIFIC VALUES -------------------------------------------------------------------------------------------

    //RANGE SELECTION
    #define R0            0x00   // Input range to -2.5/+2.5         Vref   +/- 10.24V
    #define R1            0x01   // Input range to -1.25/+1.25       Vref   +/-  5.12V
    #define R2            0x02   // Input range to -0.625/+0.625     Vref   +/-  2.56V
    #define R3            0x03   // Input range to -0.3125/+0.3125   Vref   +/-  1.28V
    #define R4            0x0B   // Input range to -0.15625/+0.15625 Vref   +/-  0.64V
    #define R5            0x05   // Input range to +2.5    Vref   10.24V
    #define R6            0x06   // Input range to +1.25   Vref    5.12V
    #define R7            0x07   // Input range to +0.625  Vref    2.56V
    #define R8            0x0F   // Input range to +0.3125 Vref    1.28V

    // OPERATION MODES
    #define MODE_IDLE       0
    #define MODE_RESET      1
    #define MODE_STANDBY    2
    #define MODE_POWER_DN   3
    #define MODE_PROG       4
    #define MODE_MANUAL     5
    #define MODE_AUTO       6
    #define MODE_AUTO_RST   7


	class ADS8688 {
		public:
            ADS8688();                                    // instantiate with PIN10 as CS
			ADS8688(uint8_t cs);                          // instantiate with custom CS pin
			
			void begin();								  // 
        
            void setVREF(float vref);                     // set external Vref
            float I2V(uint16_t x, uint8_t range);         // map uint16 to Volts according to Vref and Range
            uint16_t V2I(float x, uint8_t range);         // map Volts to uint16 according to Vref and Range

            uint16_t noOp();                              // continue previous operation
            uint16_t standBy();                           // device in stand by mode
            uint16_t powerDown();                         // power down the device
            uint16_t reset();                             // reset the device
            uint16_t autoRst();                           // reset auto sequence
            uint16_t manualChannel(uint8_t ch);           // manual channel selection

        
            void setChannelSPD(uint8_t flag);             // set channel sequence and power down other channels
            void setChannelSequence(uint8_t flag);        // set channel sequence
            void setChannelPowerDown(uint8_t flag);       // set channel power down
            uint8_t getChannelSequence();                 // get channel sequence
            uint8_t getChannelPowerDown();                // get channel sequence
        
            uint8_t getChannelRange(uint8_t ch);                      // get channel range
            void setChannelRange(uint8_t ch, uint8_t range);          // set one channel range
            void setGlobalRange(uint8_t range);                       // set all channels range
        
            uint8_t getId();                              // get device id
            void setId(uint8_t id);                       // set device id
            bool getAlarm();                              // get alarm status
            void setAlarm(bool alarm);                    // set alarm status
            uint8_t getSdo();                             // get device sdo
            void setSdo(uint8_t sdo);                     // set device sdo
            uint8_t getFeatureSelect();                                  // get feature select register
            void setFeatureSelect(uint8_t id, bool alarm, uint8_t sdo);  // set feature select register
        
            uint8_t  getAlarmOverview();                  // Logical OR of High and Low threshold per channel
            uint8_t  getFirstTrippedFlag();               // Tripped Flag remain latched until read
            uint8_t  getSecondTrippedFlag();              // Tripped Flag remain latched until read
            uint16_t getTrippedFlags();                   // Tripped Flag remain latched until read
            uint8_t  getFirstActiveFlag();                // Active Flag remains enabled as long as the alarm condition persists
            uint8_t  getSecondActiveFlag();               // Active Flag remains enabled as long as the alarm condition persists
            uint16_t getActiveFlags();                    // Active Flag remains enabled as long as the alarm condition persists

            uint8_t  getChannelHysteresis(uint8_t ch);                  //
            uint16_t getChannelLowThreshold(uint8_t ch);                //
            uint16_t getChannelHighThreshold(uint8_t ch);               //
            void     setChannelHysteresis(uint8_t ch, uint8_t val);     //
            void     setChannelLowThreshold(uint8_t ch, uint16_t val);  //
            void     setChannelHighThreshold(uint8_t ch, uint16_t val); //
        
            uint8_t  getCommandReadBack();                              // Command executed in previous data frame.
        
		private:
            float _vref;
            uint8_t _cs, _ch, _mode, _feature;                 // chip select pin, current operation mode
            void writeRegister(uint8_t reg, uint8_t val); // write 8 bit data into a register
            uint8_t readRegister(uint8_t reg);            // read 8 bit data in a specific register
            uint16_t cmdRegister(uint8_t reg);            // send a command register and read
        };

#endif // ADS8688_h
