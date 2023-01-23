#include "ac_driver_dht.h"

#define DIVIDER     80
#define SECONDS     (TIMER_BASE_CLK/DIVIDER)
#define MILLI       (SECONDS/1000)
#define MICRO       (SECONDS/(1000*1000))

#define TAG "DHT Driver"

static ac_driver_t dht_driver;
static ac_driver_function_pointer_t dht_functions[DHT_END];
static uint32_t bufferData = 0;
static uint64_t cont = 0;

static char initialize_dht_driver(void *parameters);
static char get_temperature(void *parameters);
static char get_humidity(void *parameters);

static void initPort(void);
static void initTimer(void);
static void readSensor(void);
static uint8_t check(uint32_t data, uint8_t checksum);

static char get_temperature(void *parameters)
{
  float *temperature = (float*) parameters;
  uint16_t rTemp;

  readSensor();
  
  rTemp=bufferData;
  if(0) *temperature=((rTemp>>8)+((float)(rTemp&0x00FF)/10));
  else if(1)
  {
    if(rTemp & 0x8000) *temperature=(-1)*((float)(rTemp&0x7FFF)/10);
    else *temperature=(float)(rTemp&0x7FFF)/10;
  }
  return 1;
}

static char get_humidity(void *parameters)
{
  float *humidity = (float*) parameters;
  uint16_t rHumi;

  readSensor();

  rHumi = (bufferData >> 16);
  if (0) *humidity = ((rHumi>>8) + ((float) (rHumi & 0x00FF) / 10));
  else if (1) *humidity = (float) rHumi / 10;
  return 1;
}

static char initialize_dht_driver(void *parameters)
{
    initPort();
    initTimer();
    dht_driver.driver_id = (int) parameters;
    ESP_LOGI(TAG, "Driver inicializado");
    return 1;
}

ac_driver_t* ac_get_dht_driver(void)
{
  dht_driver.driver_initialization = initialize_dht_driver;
  dht_functions[DHT_TEMPERATURE] = get_temperature;
  dht_functions[DHT_HUMIDITY] = get_humidity;
  dht_driver.driver_function = &dht_functions[0]; //Estudar.
  ESP_LOGI(TAG, "Get driver");
  return &dht_driver;
}

static void initPort(void)
{
    gpio_pad_select_gpio(PIN);
    gpio_set_direction(PIN, GPIO_MODE_INPUT_OUTPUT);
    gpio_pulldown_dis(PIN);
    gpio_pullup_dis(PIN);
    gpio_set_level(PIN, 1);
}

static void initTimer(void)
{
    timer_config_t config = 
    {
        .divider = DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = TIMER_AUTORELOAD_EN,
    };
    timer_init(GRP, TIM, &config);
}

static void readSensor(void)
{
  bufferData = 0;
    
  uint8_t state = 1;
  uint8_t nBit = 0;
  uint64_t auxCont = 0;
  uint64_t startTime = 0;
  uint8_t bufferChecksum = 0;

  gpio_set_direction(PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(PIN, 0);
  timer_set_counter_value(GRP, TIM, 0);
  timer_start(GRP, TIM);
    
  if(0) startTime = 18*MILLI;
  else if(1) startTime = 2*MILLI;

  while(state)
  {
    timer_get_counter_value(GRP, TIM, &cont);
    if (cont > 30*MILLI) {
      state = 0;
      timer_pause(GRP, TIM);
      ESP_LOGE("DHT", "Timeout");
    } else if (state == 1) {
      if(cont > startTime) {
        gpio_set_level(PIN, 1);
        timer_get_counter_value(GRP, TIM, &auxCont);
        gpio_set_direction(PIN, GPIO_MODE_INPUT);
        state = 2;
      }
    }
    else if(state == 2)
    {
      uint8_t aux01 = ((cont-auxCont)/MICRO) > (200*MICRO);
      if(aux01)
      {
        if(!gpio_get_level(PIN)) state = 3;
      }
    }
    else if(state == 3)
    {
      if(gpio_get_level(PIN))
      {
        timer_get_counter_value(GRP, TIM, &auxCont);
        state = 4;
      }
    }
    else if(state == 4)
    {
      if(!gpio_get_level(PIN))
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
          timer_pause(GRP, TIM);
          if(check(bufferData, bufferChecksum)) ESP_LOGI(TAG, "Read Success");
          else ESP_LOGE(TAG, "Reading error");
        }
      }
    }
  }
}

static uint8_t check(uint32_t data, uint8_t checksum)
{
  uint8_t sum  = (uint8_t)(data>>8)+(uint8_t)(data>>0)+
                 (uint8_t)(data>>24)+(uint8_t)(data>>16);
  if(sum == checksum) sum = 1;
  else sum = 0;
  return sum;
}