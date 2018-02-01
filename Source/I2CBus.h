/**
 * @file I2CBus.h
 * 
 * On a PocketCHIP, I2CBus accesses the i2c bus to read information on battery 
 * state or put the system in FEL mode.  
 */
#pragma once
#include <exception>
#include "../JuceLibraryCode/JuceHeader.h"

class I2CBus {
public:
    I2CBus();
    virtual ~I2CBus();

    /**
     * Read battery charge state from the i2c bus
     * @return true iff the battery is currently charging.
     * @throws I2CException if unable to access the i2c bus
     */
    bool batteryIsCharging();
    
    /**
     * Read battery charge percentage from the i2c bus
     * @return the percent of battery life remaining, between 0-100
     * @throws I2CException if unable to access the i2c bus
     */
    int batteryGaugePercent();
    
    /**
     * Writes a series of bytes to the i2c bus that will force the system to
     * enter Fel (Flashing) mode after the next restart.
     * @throws I2CException if unable to access the i2c bus
     */
    void enableFelMode();
    
    /**
     * I2CException is thrown whenever accessing the i2c bus fails.
     */
    struct I2CException : public std::exception {
    public:
        I2CException(String errorMessage);
        String getErrorMessage();
    private:
        String errorMessage;
    };

private:
    /**
     * Opens access to the i2c bus file
     * @post i2cFileDescriptor will be set to a valid file descriptor, if
     * the file could be opened.
     * @throws I2CException if unable to access the i2c bus
     */
    void i2cOpen();

    /**
     * If the i2c bus file was opened, this will close it. Otherwise, nothing
     * will happen.
     */
    void i2cClose();

    /**
     * Reads one byte from an i2c bus register.
     * @param regAddr the address of the register to access
     * @param buf the address to copy data from the register
     * @return the byte read from the register
     * @throws I2CException if unable to access the i2c bus
     */
    uint8_t i2cReadByte(uint8_t regAddr);

    /**
     * Writes a byte to an i2c bus register.
     * @param regAddr the address of the register to access
     * @param byte the data to write to the register
     * @param length number of bytes to copy
     * @throws I2CException if unable to access the i2c bus
     */
    void i2cWriteByte(uint8_t regAddr, uint8_t byte);

    int i2cFileDescriptor = -1;

    //Path to the i2c bus device file. 
    static constexpr const char* i2cPath = "/dev/i2c-0";
    //register addresses:
    static constexpr const uint8_t regCharging = 0x01;
    static constexpr const uint8_t regBattGauge = 0xB9;
    static const std::vector<std::pair<uint8_t, char>> felModeSequence;

};