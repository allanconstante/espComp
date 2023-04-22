#include "ac_driver_max30100.h"

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FRQ 100000
#define I2C_PORT I2C_NUM_0

#define TAG "Max30100 Driver"

static ac_driver_t max30100_driver;
static ac_driver_function_pointer_t max30100_functions[END_MAX30100];

static char initialize_max30100_driver(void *parameters);
static char get_raw_data(void *parameters);
static char start_temperature_reading(void *parameters);
static char is_temperature_ready(void *parametrs);
static char get_temperature(void *parameters);

static void initI2C(void);
static void initMax30100(void);
static void readRegister( i2c_port_t i2c_port, uint8_t address, uint8_t* reg, uint8_t size );
static void writeRegister( i2c_port_t i2c_port, uint8_t address, uint8_t val );

static char get_raw_data(void *parameters)
{
    uint8_t *raw = (uint8_t*) parameters;
    readRegister(I2C_PORT, FIFO_DATA_REGISTER, raw, 4);
    return 1;
}

static char start_temperature_reading(void *parameters)
{
    uint8_t data;
    readRegister( I2C_PORT, MODE_CONFIGURATION, &data, 1 );
    writeRegister( I2C_PORT, MODE_CONFIGURATION, (data | (1<<3)) ); 
    return 1;
}

static char is_temperature_ready(void *parameters)
{
    uint8_t current;
    uint8_t *ready = (uint8_t*) parameters;
    readRegister( I2C_PORT, MODE_CONFIGURATION, &current, 1 );
    *ready = (current & (1<<3));
    return 1;
}

static char get_temperature(void *parameters)
{
    int8_t temp;
    int8_t temp_fraction;
    float *temperature = (float*) parameters;
    readRegister(I2C_PORT, TEMP_INTEGER, (uint8_t*)&temp, 1);
    readRegister( I2C_PORT, TEMP_FRACTION, (uint8_t*)&temp_fraction, 1);
    *temperature = (float)temp+(float)(temp_fraction*0.0625);
    return 1;
}

static char initialize_max30100_driver(void *parameters)
{
    initI2C();
    initMax30100();
    max30100_driver.driver_id = (int) parameters;
    ESP_LOGI(TAG, "Driver inicializado");
    return 1;
}

ac_driver_t* ac_get_max30100_driver(void)
{
  max30100_driver.driver_initialization = initialize_max30100_driver;
  max30100_functions[MAX30100_GET_RAW_DATA] = get_raw_data;
  max30100_functions[MAX30100_GET_TEMPERATURE] = get_temperature;
  max30100_functions[MAX30100_START_TEMPERTURA_READING] = start_temperature_reading;
  max30100_functions[MAX30100_IS_TEMPERATURE_READY] = is_temperature_ready;
  max30100_driver.driver_function = &max30100_functions[0]; //Estudar.
  ESP_LOGI(TAG, "Get driver");
  return &max30100_driver;
}

static void initI2C(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FRQ,
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
}

static void initMax30100(void)
{
    uint8_t current;

    // Set mode ------------
    readRegister( I2C_PORT, MODE_CONFIGURATION, &current, 1 );
    writeRegister( I2C_PORT, MODE_CONFIGURATION, (current & 0xF8) | SPO2_HR_MODE );
    // ---------------------

    // Set sampling rate ---
    readRegister( I2C_PORT, SPO2_CONFIGURATION, &current, 1 );
    writeRegister( I2C_PORT, SPO2_CONFIGURATION, (current & 0xE3) | (SAMPLING_50HZ<<2) );
    // ---------------------

    // Set pulse width -----
    readRegister( I2C_PORT, SPO2_CONFIGURATION, &current, 1 );
    writeRegister( I2C_PORT, SPO2_CONFIGURATION, (current & 0xFC) | PULSE_WIDTH_1600US_ADC_16 );
    // ---------------------

    // Set LED current -----
    writeRegister( I2C_PORT, LED_CONFIGURATION, (LED_CURRENT_27_1MA << 4) | LED_CURRENT_50MA );
    // ---------------------

    // Set high res --------  
    readRegister( I2C_PORT, SPO2_CONFIGURATION, &current, 1);
    writeRegister( I2C_PORT, SPO2_CONFIGURATION, current | (1<<6) );
    // ---------------------

}

static void readRegister( i2c_port_t i2c_port, uint8_t address, uint8_t* reg, uint8_t size )
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

static void writeRegister( i2c_port_t i2c_port, uint8_t address, uint8_t val )
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