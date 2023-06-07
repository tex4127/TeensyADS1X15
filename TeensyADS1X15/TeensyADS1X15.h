//////////////////////////////////////////////////////////
///
//  Copyright (c) 2023
//  Author: Jacob Garner, mlgtex4127@gmail.com
//  
//  Filename: TeensyADS1X15.h
//
//  Description:
//  This code is designed to integrate functionality of the ADS1015 and ADS1115 chip manufactured
//  by Texas Instruments for the Teensy. I2C functionality is now supported for use with 
//  the Teensy 4.1 micro controller.
///
//////////////////////////////////////////////////////////

#pragma once

#ifndef TeensyADS1015_HH
#define TeensyADS1015_HH

//include the I2CBus Drivers for Teensy

#include "Arduino.h"
#include <TeensyI2CDevice.h>
#include <TeensyI2CBusIORegister.h>

/*=========================================================================
	I2C ADDRESS/BITS
	-----------------------------------------------------------------------*/
#define ADS1X15_ADDRESS (0x48) ///< 1001 000 (ADDR = GND)
	/*=========================================================================*/

	/*=========================================================================
		POINTER REGISTER
		-----------------------------------------------------------------------*/
#define ADS1X15_REG_POINTER_MASK (0x03)      ///< Point mask
#define ADS1X15_REG_POINTER_CONVERT (0x00)   ///< Conversion
#define ADS1X15_REG_POINTER_CONFIG (0x01)    ///< Configuration
#define ADS1X15_REG_POINTER_LOWTHRESH (0x02) ///< Low threshold
#define ADS1X15_REG_POINTER_HITHRESH (0x03)  ///< High threshold
		/*=========================================================================*/

		/*=========================================================================
			CONFIG REGISTER
			-----------------------------------------------------------------------*/
#define ADS1X15_REG_CONFIG_OS_MASK (0x8000) ///< OS Mask
#define ADS1X15_REG_CONFIG_OS_SINGLE                                           \
  (0x8000) ///< Write: Set to start a single-conversion
#define ADS1X15_REG_CONFIG_OS_BUSY                                             \
  (0x0000) ///< Read: Bit = 0 when conversion is in progress
#define ADS1X15_REG_CONFIG_OS_NOTBUSY                                          \
  (0x8000) ///< Read: Bit = 1 when device is not performing a conversion

#define ADS1X15_REG_CONFIG_MUX_MASK (0x7000) ///< Mux Mask
#define ADS1X15_REG_CONFIG_MUX_DIFF_0_1                                        \
  (0x0000) ///< Differential P = AIN0, N = AIN1 (default)
#define ADS1X15_REG_CONFIG_MUX_DIFF_0_3                                        \
  (0x1000) ///< Differential P = AIN0, N = AIN3
#define ADS1X15_REG_CONFIG_MUX_DIFF_1_3                                        \
  (0x2000) ///< Differential P = AIN1, N = AIN3
#define ADS1X15_REG_CONFIG_MUX_DIFF_2_3                                        \
  (0x3000) ///< Differential P = AIN2, N = AIN3
//ADITIONAL DIFF MEASUREMENTS
//#define ADS1X15_REG_CONFIG_MUX_DIFF_0_2 () //AIN0 = P, AIN2 = N
//#define ADS1X15_REG_CONFIG_MUX_DIFF_1_2 () //AIN0 = P, AIN2 = N


#define ADS1X15_REG_CONFIG_MUX_SINGLE_0 (0x4000) ///< Single-ended AIN0		/adjusted to 0x0000 to compare ain0 and ain1
#define ADS1X15_REG_CONFIG_MUX_SINGLE_1 (0x5000) ///< Single-ended AIN1
#define ADS1X15_REG_CONFIG_MUX_SINGLE_2 (0x6000) ///< Single-ended AIN2
#define ADS1X15_REG_CONFIG_MUX_SINGLE_3 (0x7000) ///< Single-ended AIN3

#define ADS1X15_REG_CONFIG_PGA_MASK (0x0E00)   ///< PGA Mask
#define ADS1X15_REG_CONFIG_PGA_6_144V (0x0000) ///< +/-6.144V range = Gain 2/3
#define ADS1X15_REG_CONFIG_PGA_4_096V (0x0200) ///< +/-4.096V range = Gain 1
#define ADS1X15_REG_CONFIG_PGA_2_048V                                          \
  (0x0400) ///< +/-2.048V range = Gain 2 (default)
#define ADS1X15_REG_CONFIG_PGA_1_024V (0x0600) ///< +/-1.024V range = Gain 4
#define ADS1X15_REG_CONFIG_PGA_0_512V (0x0800) ///< +/-0.512V range = Gain 8
#define ADS1X15_REG_CONFIG_PGA_0_256V (0x0A00) ///< +/-0.256V range = Gain 16

#define ADS1X15_REG_CONFIG_MODE_MASK (0x0100)   ///< Mode Mask
#define ADS1X15_REG_CONFIG_MODE_CONTIN (0x0000) ///< Continuous conversion mode
#define ADS1X15_REG_CONFIG_MODE_SINGLE                                         \
  (0x0100) ///< Power-down single-shot mode (default)

#define ADS1X15_REG_CONFIG_RATE_MASK (0x00E0) ///< Data Rate Mask

#define ADS1X15_REG_CONFIG_CMODE_MASK (0x0010) ///< CMode Mask
#define ADS1X15_REG_CONFIG_CMODE_TRAD                                          \
  (0x0000) ///< Traditional comparator with hysteresis (default)
#define ADS1X15_REG_CONFIG_CMODE_WINDOW (0x0010) ///< Window comparator

#define ADS1X15_REG_CONFIG_CPOL_MASK (0x0008) ///< CPol Mask
#define ADS1X15_REG_CONFIG_CPOL_ACTVLOW                                        \
  (0x0000) ///< ALERT/RDY pin is low when active (default)
#define ADS1X15_REG_CONFIG_CPOL_ACTVHI                                         \
  (0x0008) ///< ALERT/RDY pin is high when active

#define ADS1X15_REG_CONFIG_CLAT_MASK                                           \
  (0x0004) ///< Determines if ALERT/RDY pin latches once asserted
#define ADS1X15_REG_CONFIG_CLAT_NONLAT                                         \
  (0x0000) ///< Non-latching comparator (default)
#define ADS1X15_REG_CONFIG_CLAT_LATCH (0x0004) ///< Latching comparator

#define ADS1X15_REG_CONFIG_CQUE_MASK (0x0003) ///< CQue Mask
#define ADS1X15_REG_CONFIG_CQUE_1CONV                                          \
  (0x0000) ///< Assert ALERT/RDY after one conversions
#define ADS1X15_REG_CONFIG_CQUE_2CONV                                          \
  (0x0001) ///< Assert ALERT/RDY after two conversions
#define ADS1X15_REG_CONFIG_CQUE_4CONV                                          \
  (0x0002) ///< Assert ALERT/RDY after four conversions
#define ADS1X15_REG_CONFIG_CQUE_NONE                                           \
  (0x0003) ///< Disable the comparator and put ALERT/RDY in high state (default)
			/*=========================================================================*/

			/** Gain settings */
typedef enum {
	GAIN_TWOTHIRDS = ADS1X15_REG_CONFIG_PGA_6_144V,
	GAIN_ONE = ADS1X15_REG_CONFIG_PGA_4_096V,
	GAIN_TWO = ADS1X15_REG_CONFIG_PGA_2_048V,
	GAIN_FOUR = ADS1X15_REG_CONFIG_PGA_1_024V,
	GAIN_EIGHT = ADS1X15_REG_CONFIG_PGA_0_512V,
	GAIN_SIXTEEN = ADS1X15_REG_CONFIG_PGA_0_256V
} adsGain_t;

/** Data rates */
#define RATE_ADS1015_128SPS (0x0000)  ///< 128 samples per second
#define RATE_ADS1015_250SPS (0x0020)  ///< 250 samples per second
#define RATE_ADS1015_490SPS (0x0040)  ///< 490 samples per second
#define RATE_ADS1015_920SPS (0x0060)  ///< 920 samples per second
#define RATE_ADS1015_1600SPS (0x0080) ///< 1600 samples per second (default)
#define RATE_ADS1015_2400SPS (0x00A0) ///< 2400 samples per second
#define RATE_ADS1015_3300SPS (0x00C0) ///< 3300 samples per second

#define RATE_ADS1115_8SPS (0x0000)   ///< 8 samples per second
#define RATE_ADS1115_16SPS (0x0020)  ///< 16 samples per second
#define RATE_ADS1115_32SPS (0x0040)  ///< 32 samples per second
#define RATE_ADS1115_64SPS (0x0060)  ///< 64 samples per second
#define RATE_ADS1115_128SPS (0x0080) ///< 128 samples per second (default)
#define RATE_ADS1115_250SPS (0x00A0) ///< 250 samples per second
#define RATE_ADS1115_475SPS (0x00C0) ///< 475 samples per second
#define RATE_ADS1115_860SPS (0x00E0) ///< 860 samples per second

//ICP read registers
#define ICPWriteVoltsContChannel1 (0b1100011001001010) //sets channel 1 to continuous mode for ICP usage
#define ICPWriteVoltsContChannel1_Stop  (0b1100011011001010) //Reads channel 1 to determine if conversion is complete
#define ICPWriteVoltsContChannel2 (0b1100011001001000) //sets channel 2 to continuous mode for ICP usage
#define ICPWriteVoltsContChannel2_Stop  (0b1100011011001000) //sets channel 2 to continuous mode for ICP usage
#define ICPWriteVoltsContChannel3 (0b1100011001001110) //sets channel 2 to continuous mode for ICP usage
#define ICPWriteVoltsContChannel3_Stop  (0b1100011011001110) //sets channel 2 to continuous mode for ICP usage





class Teensy_ADS1X15
{
protected:
	TeensyI2CDevice* m_i2c_dev;
	uint8_t m_bitShift;            ///< bit shift amount
	adsGain_t m_gain;              ///< ADC gain
	uint16_t m_dataRate;           ///< Data rate
public:
	bool begin(uint8_t i2cBus);
	int16_t readADC_SingleEnded(uint8_t channel);
	//int16_t readADC_Differential_0_1();
	//int16_t readADC_Differential_2_3();
	//void startComparator_SingleEnded(uint8_t channel, int16_t threshold);
	int16_t readADCDifferential(int msrChannel);
	int16_t getLastConversionResults();
	float computeVolts(int16_t counts);
	void setGain(adsGain_t gain);
	adsGain_t getGain();
	void setDataRate(uint16_t rate);
	uint16_t getDataRate();
	float readVolts(uint8_t channel);
	bool getBegun();
	uint32_t getClock();
	void setClock(uint32_t freq);
	//void ReadVolts();

private:
	bool conversionComplete();
	void writeRegister(uint8_t reg, uint16_t value);
	uint16_t readRegister(uint8_t reg);
	uint8_t buffer[3];
	float ICPVolts[3];
	bool begun;
};


class Teensy_ADS1015 : public Teensy_ADS1X15
{
public:
	Teensy_ADS1015();
};

class Teensy_ADS1115 : public Teensy_ADS1X15
{
public:
	Teensy_ADS1115();
};



#endif