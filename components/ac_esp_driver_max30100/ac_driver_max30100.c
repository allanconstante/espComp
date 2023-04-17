
#include "ac_driver_max30100.h"

void readRegister( i2c_port_t i2c_port, uint8_t address, uint8_t* reg, uint8_t size )
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DEVICE_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, address, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DEVICE_ADDRESS << 1) | I2C_MASTER_READ, true);
    if(size > 1) i2c_master_read(cmd, reg, size-1, I2C_MASTER_ACK); //0 is ACK
    i2c_master_read_byte(cmd, reg+size-1, I2C_MASTER_NACK); //1 is NACK
    i2c_master_stop(cmd);
    i2c_master_cmd_begin( i2c_port, cmd, 1000 / portTICK_RATE_MS );
    i2c_cmd_link_delete(cmd);
}

void writeRegister( i2c_port_t i2c_port, uint8_t address, uint8_t val )
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DEVICE_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, address, true); // send register address
    i2c_master_write_byte(cmd, val, true); // send value to write
    i2c_master_stop(cmd);
    i2c_master_cmd_begin( i2c_port, cmd, 1000 / portTICK_RATE_MS );
    i2c_cmd_link_delete(cmd);
}

void initialize_max30100(i2c_port_t i2c_port)
{
    uint8_t current;

    // Set mode ------------
    readRegister( i2c_port, MODE_CONFIGURATION, &current, 1 );
    writeRegister( i2c_port, MODE_CONFIGURATION, (current & 0xF8) | SPO2_HR_MODE );
    // ---------------------

    // Set sampling rate ---
    readRegister( i2c_port, SPO2_CONFIGURATION, &current, 1 );
    writeRegister( i2c_port, SPO2_CONFIGURATION, (current & 0xE3) | (SAMPLING_50HZ<<2) );
    // ---------------------

    // Set pulse width -----
    readRegister( i2c_port, SPO2_CONFIGURATION, &current, 1 );
    writeRegister( i2c_port, SPO2_CONFIGURATION, (current & 0xFC) | PULSE_WIDTH_1600US_ADC_16 );
    // ---------------------

    // Set LED current -----
    writeRegister( i2c_port, LED_CONFIGURATION, (LED_CURRENT_27_1MA << 4) | LED_CURRENT_50MA );
    // ---------------------

    // Set high res --------  
    readRegister( i2c_port, SPO2_CONFIGURATION, &current, 1);
    writeRegister( i2c_port, SPO2_CONFIGURATION, current | (1<<6) );
    // ---------------------
}