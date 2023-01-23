#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static char get_temperature(void *parameters);
static char get_humidity(void *parameters);
static char get_voltage(void *parameters);
static char get_current(void *parameters);
static char get_pressure(void *parameters);
static char get_luminosity(void *parameters);
static char get_distance(void *parameters);
static char get_speed(void *parameters);
static char get_acceleration(void *parameters);
static char get_rotation(void *parameters);
static char get_force(void *parameters);
static char get_torque(void *parameters);
static char get_flow(void *parameters);

#ifdef __cplusplus
}
#endif