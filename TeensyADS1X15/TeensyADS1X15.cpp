//////////////////////////////////////////////////////////
///
//  Copyright (c) 2023
//  Author: Jacob Garner, mlgtex4127@gmail.com
//  
//  Filename: TeensyADS1X15.cpp
//
//  Description:
//  This code is designed to integrate functionality of the ADS1015 and ADS1115 chip manufactured
//  by Texas Instruments for the Teensy. I2C functionality is now supported for use with 
//  the Teensy 4.1 micro controller.
///
//////////////////////////////////////////////////////////

#include "TeensyADS1X15.h"

Teensy_ADS1015::Teensy_ADS1015()
{
	m_bitShift = 4;
	m_gain = GAIN_TWOTHIRDS;
	m_dataRate = RATE_ADS1015_1600SPS;
}

Teensy_ADS1115::Teensy_ADS1115()
{
	m_bitShift = 0;
	m_gain = GAIN_TWOTHIRDS;
	m_dataRate = RATE_ADS1115_128SPS;
}

bool Teensy_ADS1X15::begin( uint8_t i2cBus)
{
	m_i2c_dev = new TeensyI2CDevice(ADS1X15_ADDRESS, i2cBus);
    begun = m_i2c_dev->begin(); //this is done to call the status of begin at every point IF the communication could be sent/received
    return begun;
}

void Teensy_ADS1X15::setGain(adsGain_t gain) { m_gain = gain; }
adsGain_t Teensy_ADS1X15::getGain() { return m_gain; }
void Teensy_ADS1X15::setDataRate(uint16_t rate) { m_dataRate = rate; }
uint16_t Teensy_ADS1X15::getDataRate() { return m_dataRate; }
bool Teensy_ADS1X15::getBegun() { return begun; }
uint32_t Teensy_ADS1X15::getClock() { return m_i2c_dev->m_wire->getClock(); }
void Teensy_ADS1X15::setClock(uint32_t freq) { m_i2c_dev->m_wire->setClock(freq); }

int16_t Teensy_ADS1X15::readADC_SingleEnded(uint8_t channel) {
    if (channel > 3) {
        return 0;
    }

    // Start with default values
    uint16_t config =
        ADS1X15_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
        ADS1X15_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
        ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1X15_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
        ADS1X15_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= m_gain;

    // Set data rate
    config |= m_dataRate;

    // Set single-ended input channel
    switch (channel) {
    case (0):
        config |= ADS1X15_REG_CONFIG_MUX_SINGLE_0;
        break;
    case (1):
        config |= ADS1X15_REG_CONFIG_MUX_SINGLE_1;
        break;
    case (2):
        config |= ADS1X15_REG_CONFIG_MUX_SINGLE_2;
        break;
    case (3):
        config |= ADS1X15_REG_CONFIG_MUX_SINGLE_3;
        break;
    }

    // Set 'start single-conversion' bit
    config |= ADS1X15_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(ADS1X15_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    while (!conversionComplete())
        ;

    // Read the conversion results
    return getLastConversionResults();
}


//read the voltage from each of the approrpiate channels in continusous mode
//void Teensy_ADS1X15::ReadVolts()
//{
//    //to even out the readings, take 4 samples and report back the average as x += 0.25 * result | store in an array
//    //start cont mode
//    for (int j = 0; j< 3; j++){ICPVolts[j] = 0.0;}
//    for (int i = 0; i < 4; i++)
//    {
//        writeRegister(ADS1X15_REG_POINTER_CONFIG, ICPWriteVoltsContChannel1);
//        //while(!conversionComplete()) {/*Do nothing*/}
//        writeRegister(ADS1X15_REG_POINTER_CONFIG, ICPWriteVoltsContChannel1_Stop);
//        ICPVolts[0] += 0.25 * computeVolts(getLastConversionResults());
//        writeRegister(ADS1X15_REG_POINTER_CONFIG, ICPWriteVoltsContChannel2);
//        //while(!conversionComplete()){/*Do nothing*/}
//        writeRegister(ADS1X15_REG_POINTER_CONFIG, ICPWriteVoltsContChannel1_Stop);
//       ICPVolts[1] += 0.25 * computeVolts(getLastConversionResults());
//        writeRegister(ADS1X15_REG_POINTER_CONFIG, ICPWriteVoltsContChannel3);
//        //while(!conversionComplete()) {/*Do nothing*/}
//        writeRegister(ADS1X15_REG_POINTER_CONFIG, ICPWriteVoltsContChannel1_Stop);
//        ICPVolts[2] += 0.25 * computeVolts(getLastConversionResults());
//    }
//}


int16_t Teensy_ADS1X15::getLastConversionResults() {
    // Read the conversion results
    uint16_t res = readRegister(ADS1X15_REG_POINTER_CONVERT) >> m_bitShift;
    if (m_bitShift == 0) {
        return (int16_t)res;
    }
    else {
        // Shift 12-bit results right 4 bits for the ADS1015,
        // making sure we keep the sign bit intact
        if (res > 0x07FF) {
            // negative number - extend the sign to 16th bit
            res |= 0xF000;
        }
        return (int16_t)res;
    }
}


float Teensy_ADS1X15::computeVolts(int16_t counts) {
    // see data sheet Table 3
    float fsRange;
    switch (m_gain) {
    case GAIN_TWOTHIRDS:
        fsRange = 6.144f;
        break;
    case GAIN_ONE:
        fsRange = 4.096f;
        break;
    case GAIN_TWO:
        fsRange = 2.048f;
        break;
    case GAIN_FOUR:
        fsRange = 1.024f;
        break;
    case GAIN_EIGHT:
        fsRange = 0.512f;
        break;
    case GAIN_SIXTEEN:
        fsRange = 0.256f;
        break;
    default:
        fsRange = 0.0f;
    }
    return counts * (fsRange / (32768 >> m_bitShift));
}


bool Teensy_ADS1X15::conversionComplete() {
    return (readRegister(ADS1X15_REG_POINTER_CONFIG) & 0x8000) != 0;
}

void Teensy_ADS1X15::writeRegister(uint8_t reg, uint16_t value) {
    buffer[0] = reg;
    buffer[1] = value >> 8;
    buffer[2] = value & 0xFF;
    m_i2c_dev->write(buffer, 3);
}

uint16_t Teensy_ADS1X15::readRegister(uint8_t reg) {
    buffer[0] = reg;
    m_i2c_dev->write(buffer, 1);
    m_i2c_dev->read(buffer, 2);
    return ((buffer[0] << 8) | buffer[1]);
}

/// <summary>
/// Read the channels and convert the bits to a voltage in one line
/// </summary>
/// <param name="channel"></param>
/// <returns></returns>
float Teensy_ADS1X15::readVolts(uint8_t channel)
{
    //single line conversion using pre-existing functiosn
    return Teensy_ADS1X15::computeVolts(Teensy_ADS1X15::readADC_SingleEnded(channel));
}

int16_t Teensy_ADS1X15::readADCDifferential(int msrChannel)
{
    uint16_t config =
        ADS1X15_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
        ADS1X15_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
        ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1X15_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
        ADS1X15_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    //set the gain
    config |= m_gain;
    //set the data rate
    config |= m_dataRate;

    //setup switch case to handle the channel parameter
    switch (msrChannel) 
    {
    case 0:
        //set config for Diff from MUX
        //config |= ADS1X15_REG_CONFIG_MUX_DIFF_0_2;
        //set the start conversion bit
        //config |= ADS1X15_REG_CONFIG_OS_SINGLE;
        break;
    case 1:
        //set config for Diff from MUX
        //config |= ADS1X15_REG_CONFIG_MUX_DIFF_1_2;
        //set the start conversion bit
        //config |= ADS1X15_REG_CONFIG_OS_SINGLE;
        break;
    case 2:
        config |= ADS1X15_REG_CONFIG_MUX_SINGLE_2;
        //set the start conversion bit
        config |= ADS1X15_REG_CONFIG_OS_SINGLE;
        return 0;
        break;
    case 3:
        //set config for Diff from MUX
        config |= ADS1X15_REG_CONFIG_MUX_DIFF_2_3;
        //set the start conversion bit
        config |= ADS1X15_REG_CONFIG_OS_SINGLE;
        break;
    default:
        return 0;
    }
        
    //write the register
    writeRegister(ADS1X15_REG_POINTER_CONFIG, config);

    while (!conversionComplete());
    ;
    return (-getLastConversionResults());

}