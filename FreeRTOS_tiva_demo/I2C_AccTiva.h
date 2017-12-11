/*
 * I2C_LightTiva.h
 *
 *  Created on: Dec 2, 2017
 *      Author: Saritha
 */

#ifndef I2C_ACCTIVA_H_
#define I2C_ACCTIVA_H_

#include "inc/hw_memmap.h"
#include "i2c.h"
#include "gpio.h"
#include "driverlib/pin_map.h"

#define I2C_ACC_SLAVE_ADDR                  (0x1D)
#define I2C_CMD_CODE_ACC                    (0x80)
#define I2C_CMD_CODE                        (0x80)
#define I2C_ACC_ID_REG                      (0x0D)
#define I2C_ACC_CONTROL_REG2                (0x2B)
#define I2C_ACC_CONTROL_REG2_VAL            (0x40)
#define I2C_FF_MT_CFG                       (0x15)
#define I2C_FF_MT_CFG_VAL                   (0x78)
#define I2C_FF_MT_THS                       (0x17)
#define I2C_FF_MT_THS_VAL                   (0x10)
#define I2C_FF_MT_COUNT                     (0x18)
#define I2C_FF_MT_COUNT_VAL                 (0x0A)
#define I2C_ACC_CONTROL_REG4                (0x2D)
#define I2C_ACC_CONTROL_REG4_VAL            (0x04)
#define I2C_ACC_CONTROL_REG1                (0x2A)
#define I2C_ACC_CONTROL_REG1_VAL            (0xF9)
#define I2C_INT_SOURCE                      (0x0C)





#define ONE_BYTE                (1)
#define TWO_BYTE                (2)


/**
 * @brief
 *      This function is used to setup the Acc sensor
 * @return
 *      int8_t - to check the return status
 */
int8_t i2cAccSetup();

/**
 * @brief
 *      This function is used to write to the control register
 * @param
 *      value - to be written to the control register
 * @return
 *      int8_t - to check the return status
 */
int8_t write_control1_reg_acc(uint8_t value);

/**
 * @brief
 *      This function is used to write to the register specified
 * @param
 *      reg_value - to be written to the register address
 * @return
 *      int8_t - to check the return status
 */
int8_t write_reg_acc(int8_t reg_addr, int8_t reg_value);

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
int8_t read_data_acc(int8_t reg_addr, uint8_t *data_val, uint8_t num_bytes);
int8_t write_control1_reg_acc(uint8_t value);
int8_t write_control2_reg_acc(uint8_t value);
int8_t write_ff_mt_cfg_reg_acc(uint8_t value);
int8_t write_ff_mt_ths_reg_acc(uint8_t value);
int8_t write_ff_mt_count_reg_acc(uint8_t value);
int8_t write_control4_reg_acc(uint8_t value);


#endif /* I2C_LIGHTTIVA_H_ */
