/**
  ******************************************************************************
  * @file           : .c
  * @brief          : .
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Private includes ----------------------------------------------------------*/
#include "br.com.aconstante.dht.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define DIVIDER     80
#define SECONDS     (TIMER_BASE_CLK/DIVIDER)
#define MILLI       (SECONDS/1000)
#define MICRO       (SECONDS/(1000*1000))

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t  bufferChecksum = 0;
uint8_t  modelGlobal;
uint32_t bufferData = 0;
uint64_t cont = 0;
timer_group_t groupGlobal;
timer_idx_t timerGlobal;
gpio_num_t pinGobal;

/* Private function prototypes -----------------------------------------------*/
void initPort(void);
void initTimer(void);
void readSensor(void);
float toConvert(uint8_t, uint8_t);
uint8_t check(uint32_t, uint8_t);

/* Private user code ---------------------------------------------------------*/
void getRawData(uint32_t *data, uint8_t *checksum)
{
    readSensor();
    *data = bufferData;
    *checksum = bufferChecksum;
}

void getSensorData(float *temperature, float *humidity)
{
    readSensor();

    uint16_t rTemp = bufferData;
    uint16_t rHumi = (bufferData >> 16);
    
    if(modelGlobal == DHT11)
    {
        *temperature = ((rTemp>>8)+((float)(rTemp & 0x00FF)/10));
    }
    else if(modelGlobal == DHT22)
    {
        if(rTemp & 0x8000)
        {
            *temperature = (-1)*((float)(rTemp & 0x7FFF)/10);
        }
        else
        {
            *temperature = (float) (rTemp & 0x7FFF)/10;
        }
    }
    
    if(modelGlobal == DHT11)
    {
        *humidity = ((rHumi>>8)+((float)(rHumi & 0x00FF)/10));
    }
    else if(modelGlobal == DHT22)
    {
        *humidity = (float) rHumi/10;
    }
}

float getTemperature(void)
{
    readSensor();

    float temperature = 0;
    uint16_t rTemp = bufferData;
    
    if(modelGlobal == DHT11)
    {
        temperature = ((rTemp>>8)+((float)(rTemp & 0x00FF)/10));
    }
    else if(modelGlobal == DHT22)
    {
        if(rTemp & 0x8000)
        {
            temperature = (-1)*((float)(rTemp & 0x7FFF)/10);
        }
        else
        {
            temperature = (float) (rTemp & 0x7FFF)/10;
        }
    }
    return temperature;
}

float getHumidity(void)
{
    readSensor();

    float humidity = 0;
    uint16_t rHumi = (bufferData >> 16);
    
    if(modelGlobal == DHT11)
    {
        humidity = ((rHumi>>8)+((float)(rHumi & 0x00FF)/10));
    }
    else if(modelGlobal == DHT22)
    {
        humidity = (float) rHumi/10;
    }
    return humidity;
}

void provSensor(timer_group_t group, 
                timer_idx_t timer, gpio_num_t pin, uint8_t model)
{
    groupGlobal = group;
    timerGlobal = timer;
    pinGobal = pin;
    modelGlobal = model;
    initPort();
    initTimer();
}

void initPort(void)
{
    gpio_pad_select_gpio(pinGobal);
    gpio_set_direction(pinGobal, GPIO_MODE_INPUT_OUTPUT);
    gpio_pulldown_dis(pinGobal);
    gpio_pullup_dis(pinGobal);
    gpio_set_level(pinGobal, 1);
}

void initTimer(void)
{
    timer_config_t config = 
    {
        .divider = DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = TIMER_AUTORELOAD_EN,
    };
    timer_init(groupGlobal, timerGlobal, &config);
}

void readSensor(void)
{
    uint8_t state = 1;
    uint8_t nBit = 0;
    uint64_t auxCont = 0;
    uint64_t startTime = 0;

    bufferData = 0;
    bufferChecksum = 0;

    gpio_set_direction(pinGobal, GPIO_MODE_OUTPUT);
    gpio_set_level(pinGobal, 0);
    timer_set_counter_value(groupGlobal, timerGlobal, 0);
    timer_start(groupGlobal, timerGlobal);
    

    if(modelGlobal == DHT11)
    {
        startTime = 18*MILLI;
    }
    else if(modelGlobal == DHT22)
    {
        startTime = 2*MILLI;
    }

    while(state)
    {
        timer_get_counter_value(groupGlobal, timerGlobal, &cont);
        if(cont > 30*MILLI)
        {
            state = 0;
            timer_pause(groupGlobal, timerGlobal);
            ESP_LOGE("DHT", "Timeout");
        }
        else if(state == 1)
        {
            if(cont > startTime)
            {
                gpio_set_level(pinGobal, 1);
                timer_get_counter_value(groupGlobal, timerGlobal, &auxCont);
                gpio_set_direction(pinGobal, GPIO_MODE_INPUT);
                state = 2;
            }
        }
        else if(state == 2)
        {
            uint8_t aux01 = ((cont-auxCont)/MICRO) > (200*MICRO);
            if(aux01)
            {
                if(!gpio_get_level(pinGobal)) state = 3;
            }
        }
        else if(state == 3)
        {
            if(gpio_get_level(pinGobal))
            {
                timer_get_counter_value(groupGlobal, timerGlobal, &auxCont);
                state = 4;
            }
        }
        else if(state == 4)
        {
            if(!gpio_get_level(pinGobal))
            {
                uint8_t aux01 = ((cont-auxCont)/MICRO) > (35*MICRO);
                if(nBit < 32)
                {
                    if(aux01) bufferData = bufferData | (1<<(31-nBit));
                    ++nBit;
                    state = 3;
                }
                else
                {
                    if(aux01) bufferChecksum = bufferChecksum | (1<<(39-nBit));
                    ++nBit;
                    state = 3;
                }
                if(nBit == 40)
                {
                    state = 0;
                    timer_pause(groupGlobal, timerGlobal);
                    if(check(bufferData, bufferChecksum))
                    {
                        ESP_LOGI("DHT", "Read Success");
                    }
                    else ESP_LOGE("DHT", "Reading error");
                }
            }
        }
    }
}

uint8_t check(uint32_t data, uint8_t checksum)
{

    uint8_t aux;
    aux = (uint8_t) (bufferData>>8)  + (uint8_t) (bufferData>>0) +
          (uint8_t) (bufferData>>24) + (uint8_t) (bufferData>>16);

    if(aux == checksum) aux = 1;
    else aux = 0;
    return aux;
}