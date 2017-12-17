/*
 * I2C_LightTiva.h
 *
 *  Created on: Dec 2, 2017
 *      Author: Saritha
 */

#ifndef I2C_LIGHTTIVA_H_
#define I2C_LIGHTTIVA_H_

#include "inc/hw_memmap.h"
#include "i2c.h"
#include "gpio.h"
#include "driverlib/pin_map.h"

#define I2C_LIGHT_SLAVE_ADDR    (0x39)
#define I2C_CONTROL_REG         (0x00)
#define I2C_CONTROL_REG_POWER   (0x03)
#define I2C_CMD_CODE            (0x80)
#define I2C_DATA0_REG           (0x0C)
#define I2C_DATA1_REG           (0x0E)

#define ONE_BYTE                (1)
#define TWO_BYTE                (2)



/**
 * @brief
 *      This function is the task function for the implementation of I2C communication for Light sensor
 * @return
 *      void
 */
void I2C_Light(void *pvParameters);

/**
 * @brief
 *      This function is used to setup the Light sensor
 * @return
 *      int8_t - to check the return status
 */
int8_t i2cLightSetup();

/**
 * @brief
 *      This function is used to write to the control register
 * @param
 *      value - to be written to the control register
 * @return
 *      int8_t - to check the return status
 */
int8_t write_control_reg(uint8_t value);

/**
 * @brief
 *      This function is used to write to the register specified
 * @param
 *      reg_value - to be written to the register address
 * @return
 *      int8_t - to check the return status
 */
int8_t write_reg(int8_t reg_addr, int8_t reg_value);

/**
 * @brief
 *      This function is used to check the ambience
 * @param
 *      light_dark - to store the boolean value is light/dark
 * @return
 *      int8_t - to check the return status
 */
int8_t light_dark(bool *light_dark);

/**
 * @brief
 *      This function is used to obtain the current lux value
 * @param
 *      lux - to store the current lux value
 * @return
 *      int8_t - to check the return status
 */
int8_t read_lux(float *lux);

/**
 * @brief
 *      This function is used to read data from the register address specified
 * @param
 *      reg_addr - to pass the register address
 *      data_val - in which the read value will be stored
 *      num_bytes - number of bytes to be read
 * @return
 *      int8_t - to check the return status
 */
int8_t read_data(int8_t reg_addr, uint8_t *data_val, uint8_t num_bytes);

/**
 * @brief
 *      This function is used to calculate the current lux value
 * @param
 *      data0 - first data value to compute lux
 *      data1 - second data value to compute lux
 *      lux - variable to store the lux value
 * @return
 *      int8_t - to check the return status
 */
int8_t lux_calculate(uint16_t data0, uint16_t data1, float *lux);

#endif /* I2C_LIGHTTIVA_H_ */
