/**
 * @file 
 *		apds_proximity.c 		*BBG*
 * @brief 
 *		This file is used for definition of the functions used for configuring, reading, writing the Proximity sensor - APDS9960
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 4,2017
 *
 */

#include "apds_proximity.h"

I2C_STATE proximity_setup()											/* I2C Device setup */
{	
	proximity_file = open(I2C_DEVICE_FILE, O_RDWR);					/* open the device file */
	if (proximity_file < 0) 										/* Check if the device file is valid */
	{
		return I2C_FAILURE;										
	}
	if (ioctl(proximity_file, I2C_SLAVE, I2C_PROXIMITY_ADDR) < 0) 			/* Specify the device addr */
	{
		return I2C_FAILURE;										
	}
	return I2C_SUCCESS;
}

I2C_STATE write_pointer_reg(uint8_t *pointer_reg_val, size_t write_data_length)	/* Common API for writing to the sensor */
{
	if(proximity_file < 0)														/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if(write(proximity_file, pointer_reg_val, write_data_length) != write_data_length)	/*  I2C common write */
	{
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE common_read(uint8_t * read_data, uint8_t pointer_reg_val, size_t read_data_length)/* Common API for reading from the sensor */
{
	if( write_pointer_reg(&pointer_reg_val,ONE_BYTE) == I2C_FAILURE)			/*  I2C write */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	if(proximity_file < 0)														/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if(read(proximity_file, read_data, read_data_length) != read_data_length)	/*  I2C common read */
	{
		printf("Read Error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_id_value(uint8_t *returned_value)						/* Read the ID of the sensor */
{
	uint8_t read_value;
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read(&read_value, I2C_ID_ADDR, ONE_BYTE) == I2C_FAILURE)		/* Read the ID reg */
	{
		printf("Failure in reading the ID reg\n");
		return I2C_FAILURE;
	}
	*returned_value =   read_value;											
	return I2C_SUCCESS;
}

I2C_STATE read_enable_reg(uint8_t *returned_value)							/* Enable register read */
{
	uint8_t read_value;
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read(&read_value, I2C_ENABLE_ADDR, ONE_BYTE) == I2C_FAILURE)		/* common read api to read the enable reg */
	{
		printf("Failure in reading the enable reg\n");
		return I2C_FAILURE;
	}
	*returned_value =   read_value;											
	return I2C_SUCCESS;
}

I2C_STATE write_enable_value(uint8_t byte1)		
{
	uint8_t enable_reg_val[2];												/*  Addr and value of the enable reg */
	enable_reg_val[0] = I2C_ENABLE_ADDR;
	enable_reg_val[1] = byte1;
	if( write_pointer_reg(enable_reg_val,TWO_BYTE) == I2C_FAILURE)			/* Write the enable reg */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_pdata_reg(uint8_t *returned_value)								/* Read the proximity data value */
{
	uint8_t read_value;
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read(&read_value, I2C_PDATA_ADDR, ONE_BYTE) == I2C_FAILURE)		/* Read the proximity pdata reg */
	{
		printf("Failure in reading the enable reg\n");
		return I2C_FAILURE;
	}
	*returned_value =   read_value;											
	return I2C_SUCCESS;
}

I2C_STATE read_control_reg(uint8_t *returned_value)				/* Control reg to read the PGAIN value */
{
	uint8_t read_value;
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read(&read_value, I2C_CONTROL_ADDR, ONE_BYTE) == I2C_FAILURE)		/* Read the control reg */
	{
		printf("Failure in reading the enable reg\n");
		return I2C_FAILURE;
	}
	*returned_value =   read_value;											
	return I2C_SUCCESS;
}

I2C_STATE write_control_value(uint8_t byte1)								/* Change the PGAIN value */
{
	uint8_t control_reg_val[2];												/*  Addr and value of the control reg */
	control_reg_val[0] = I2C_CONTROL_ADDR;
	control_reg_val[1] = byte1;
	if( write_pointer_reg(control_reg_val,TWO_BYTE) == I2C_FAILURE)			/* Write the control reg */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_pulse_len_count_reg(uint8_t *returned_value)						/* Read pulse length and count reg */
{
	uint8_t read_value;
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read(&read_value, I2C_PULSE_LEN_COUNT_ADDR, ONE_BYTE) == I2C_FAILURE)		// Read the pulse_len_count reg 
	{
		printf("Failure in reading the enable reg\n");
		return I2C_FAILURE;
	}
	*returned_value =   read_value;											
	return I2C_SUCCESS;
}

I2C_STATE write_pulse_len_count_value(uint8_t byte1)							/* Write pulse length and count reg */ 
{
	uint8_t pulse_len_reg_val[2];												/*  Addr and value of the pulse_len_count reg */
	pulse_len_reg_val[0] = I2C_PULSE_LEN_COUNT_ADDR;
	pulse_len_reg_val[1] = byte1;
	if( write_pointer_reg(pulse_len_reg_val,TWO_BYTE) == I2C_FAILURE)			/* Write the pulse_len_count reg */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}