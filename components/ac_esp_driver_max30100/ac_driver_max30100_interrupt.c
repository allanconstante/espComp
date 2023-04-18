#include "ac_driver_max30100_interrupt.h"

#define TAG "Interrupt Driver Max30100"

static ac_driver_t max30100_driver_interrupt;
static ac_driver_function_pointer_t max30100_functions_interrupt[END_MAX30100_INTERRUPT];
static ac_driver_interrupt_function_pointer_t interrupt_function_max30100;

static char initialize_max30100_driver_interrupt(void *parameters);
static char set_interrupt_function(void *parameters);
static char enable_interrupt(void *parameters);
static char disable_interrupt(void *parameters);

static char set_interrupt_function(void *parameters)
{
  interrupt_function_max30100 = (ac_driver_interrupt_function_pointer_t) parameters;
  return 1;
}

static char enable_interrupt(void *parameters)
{

}

static char disable_interrupt(void *parameters)
{

}

static char initialize_max30100_driver_interrupt(void *parameters)
{
    //init();
    max30100_driver_interrupt.driver_id = (int) parameters;
    ESP_LOGI(TAG, "Driver inicializado");
    return 1;
}

ac_driver_t* ac_get_max30100_driver_interruption(void)
{
  max30100_driver_interrupt.driver_initialization = initialize_max30100_driver_interrupt;
  max30100_functions_interrupt[SET_MAX30100_INTERRUPT] = set_interrupt_function;
  max30100_functions_interrupt[ENABLE_MAX30100_INTERRUPT] = enable_interrupt;
  max30100_functions_interrupt[DISABLE_MAX30100_INTERRUPT] = disable_interrupt;
  max30100_driver_interrupt.driver_function = &max30100_functions_interrupt[0]; //Estudar.
  ESP_LOGI(TAG, "Get driver");
  return &max30100_driver_interrupt;
}


/*
  void func_interrupt(void)
  {
    interrupt_function_max30100();
  }
*/