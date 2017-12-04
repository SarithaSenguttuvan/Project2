/**
 * @file 
 *		apds_proximity.h 		*BBG*
 * @brief 
 *		This file is used for declaration of the functions used for configuring, reading, writing the Proximity sensor - APDS9960
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 4,2017
 *
 */


#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

int proximity_file;

typedef enum{
	I2C_SUCCESS,
	I2C_FAILURE,
}I2C_STATE;

#define I2C_DEVICE_FILE 			("/dev/i2c-2")
#define I2C_PROXIMITY_ADDR			(0x39)
#define I2C_ID_ADDR					(0x92)
#define I2C_ENABLE_ADDR				(0x80)
#define I2C_ADC_INT_TIME_ADDR		(0x81)
#define I2C_PULSE_LEN_COUNT_ADDR    (0x8E)
#define I2C_CONFIG1_ADDR            (0x8D)
#define I2C_CONFIG2_ADDR            (0x90)
#define I2C_CONFIG3_ADDR            (0x9F)
#define I2C_CONFIG4_ADDR            (0xAB)
#define I2C_PROX_OFFSET_UR_ADDR     (0x9D)
#define I2C_PROX_OFFSET_DL_ADDR     (0x9E)
#define I2C_CONTROL_ADDR            (0x8F)
#define I2C_GES_ENT_TH_ADDR         (0xA0)
#define I2C_GES_EXT_TH_ADDR         (0xA1)
#define I2C_GES_CONF1_ADDR          (0xA2)
#define I2C_GES_CONF2_ADDR          (0xA3)
#define I2C_GES_CONF3_ADDR          (0xAA)
#define I2C_GES_UP_OFFSET_ADDR      (0xA4)
#define I2C_GES_DOWN_OFFSET_ADDR    (0xA5)
#define I2C_GES_LEFT_OFFSET_ADDR    (0xA7)
#define I2C_GES_RIGHT_OFFSET_ADDR   (0xA9)
#define I2C_GES_PULSE_LEN_COUNT_ADDR (0xA6)
#define I2C_GES_STATUS 				(0xAF)
#define I2C_GES_FIFO_LEVEL 			(0xAE)
#define I2C_PDATA_ADDR 				(0x9C)
#define ONE_BYTE					(1)
#define TWO_BYTE					(2)

#define ENABLE_PON_PEN 				(0x05)
#define CONTROL_REG_VAL				(0x0C)
#define PULSE_LEN_REG_VAL 			(0xC0)

I2C_STATE proximity_setup();
I2C_STATE write_pointer_reg(uint8_t *pointer_reg_val, size_t write_data_length);
I2C_STATE common_read(uint8_t * read_data, uint8_t pointer_reg_val, size_t read_data_length);
I2C_STATE read_id_value(uint8_t *returned_value);
I2C_STATE read_enable_reg(uint8_t *returned_value);
I2C_STATE write_enable_value(uint8_t byte1);
I2C_STATE read_pdata_reg(uint8_t *returned_value);
I2C_STATE read_control_reg(uint8_t *returned_value);
I2C_STATE write_control_value(uint8_t byte1);
I2C_STATE read_pulse_len_count_reg(uint8_t *returned_value);
I2C_STATE write_pulse_len_count_value(uint8_t byte1);