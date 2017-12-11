/*
 * I2C_LightTiva.c
 *
 *  Created on: Dec 2, 2017
 *      Author: Saritha
 */

#include "genericTiva.h"
#include "I2C_LightTiva.h"


extern uint32_t output_clock_rate_hz;

/* I2C setup function */
int8_t i2cLightSetup()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);         /* Enable the I2C0 peripheral */
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));  /* Wait for the I2C0 module to be ready */

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);        /* Port B */
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    GPIOPinConfigure(GPIO_PB2_I2C0SCL);                 /* PB2 SCL */
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);                 /* PB2 SDA */

    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    I2CMasterInitExpClk(I2C0_BASE, output_clock_rate_hz, false);     /* Initialize Master and Slave */
    return 0;
}

/* Write to the Control Register */
int8_t write_control_reg(uint8_t value)
{
    int8_t reg_addr = I2C_CONTROL_REG | I2C_CMD_CODE;       /* Reg address with command code */
    write_reg(reg_addr, value);     /* Write to the register */
    return 0;
}

/* Write to the register */
int8_t write_reg(int8_t write_reg_addr, int8_t reg_value)        /* Function to write the value into the Register address */
{
    uint32_t err_check = 0;
    I2CMasterSlaveAddrSet(I2C0_BASE, I2C_LIGHT_SLAVE_ADDR, false);  /* Specify slave address */
    I2CMasterDataPut(I2C0_BASE, write_reg_addr);           /* Register address */
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);    /* Initiate send of character from Master to Slave */
    while (!(I2CMasterBusy(I2C0_BASE))); /* Wait till end of transaction */
    while(I2CMasterBusy(I2C0_BASE));  /* Delay until transmission completes */

    err_check = I2CMasterErr(I2C0_BASE);
    if((err_check == I2C_MASTER_ERR_NONE))
    {
    }
    else
    {
    }

    /* Data */
    I2CMasterDataPut(I2C0_BASE, reg_value);           /* Register value */
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);    /* Initiate send of character from Master to Slave */
    while (!(I2CMasterBusy(I2C0_BASE))); /* Wait till end of transaction */
    while(I2CMasterBusy(I2C0_BASE));  /* Delay until transmission completes */

    err_check = I2CMasterErr(I2C0_BASE);
    if((err_check == I2C_MASTER_ERR_NONE))
    {
    }
    else
    {
    }
    return 0;
}

/* Function to test Light/Dark */
int8_t light_dark(bool *light_dark)
{
    float lux_val = 0;
    if(read_lux(&lux_val) != 0)                              /* Determine the lux value */
    {
        return -1;
    }
    if(lux_val > 50)                                     /* Determine if light or dark */
    {
        *light_dark = true;
    }
    else
    {
        *light_dark = false;
    }
    return 0;
}

/* Function to read the lux value */
int8_t read_lux(float *lux)
{
    uint16_t read1_Value = 0;
    uint16_t read2_Value = 0;
    read_data(I2C_DATA0_REG, (uint8_t *)&read1_Value,TWO_BYTE);     /* Read data from data register0 */
    read_data(I2C_DATA1_REG, (uint8_t *)&read2_Value,TWO_BYTE);     /* Read data from data register1 */
    lux_calculate(read1_Value,read2_Value,lux);                     /* Calculate the lux value */
    return 0;
}

/* Read data from the register address */
int8_t read_data(int8_t reg_addr, uint8_t *data_val, uint8_t num_bytes)
{
    uint8_t i = 0;
    uint32_t err_check = 0;
    *data_val = 0;
    int8_t read_reg_addr = reg_addr | I2C_CMD_CODE;             /* register addres with the command code */

    /* Writing the Address */
    I2CMasterSlaveAddrSet(I2C0_BASE, I2C_LIGHT_SLAVE_ADDR, false);  /* Specify slave address */

    I2CMasterDataPut(I2C0_BASE, read_reg_addr);                     /* Register address */

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);        /* Initiate send of character from Master to Slave */

    while (!(I2CMasterBusy(I2C0_BASE)));                            /* Wait till end of transaction */
    while(I2CMasterBusy(I2C0_BASE));                                /* Delay until transmission completes */
    err_check = I2CMasterErr(I2C0_BASE);
    if((err_check == I2C_MASTER_ERR_NONE))
    {
    }
    else
    {
    }
    /* Reading from the address */
    I2CMasterSlaveAddrSet (I2C0_BASE, I2C_LIGHT_SLAVE_ADDR, true);  /* set slave address and read */
    while(i<num_bytes)
    {
        if(num_bytes == 1)                  /* if only byte to be read */
            I2CMasterControl (I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        else if(i == 0)                     /* If first byte of two bytes */
            I2CMasterControl (I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
        else if(i == (num_bytes-1))         /* If last byte of two bytes */
            I2CMasterControl (I2C0_BASE,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

        while (!(I2CMasterBusy(I2C0_BASE)));                            /* Wait till end of transaction */
        while(I2CMasterBusy(I2C0_BASE));                                /* Delay until transmission completes */
        err_check = I2CMasterErr(I2C0_BASE);
        if((err_check == I2C_MASTER_ERR_NONE))
        {
        }
        else
        {
        }

        *(data_val+i) = (uint8_t)I2CMasterDataGet(I2C0_BASE);
        err_check = I2CMasterErr(I2C0_BASE);
        if((err_check == I2C_MASTER_ERR_NONE))
        {
        }
        else
        {
        }
        i++;
    }
    return 0;
}

/* Calculating the lux value */
int8_t lux_calculate(uint16_t data0, uint16_t data1, float *lux)
{
    float temp = 0;
    if(lux == NULL)
    {
        return -1;
    }
    temp = ((float)data1)/((float)data0);                               /* CH1/CH0 */
    if(temp <= 0.50)                                                    /* Calculation of the lux value */
    {
        *lux =  (0.0304*data0)-(0.062*data0*((data1/data0)*1.4));
    }
    else if(temp <= 0.61)
    {
        *lux =  ((0.0224*data0)-(0.031*data1));
    }
    else if(temp <= 0.80)
    {
        *lux =  ((0.0128*data0)-(0.0153*data1));
    }
    else if(temp <= 1.30)
    {
        *lux =  ((0.00146*data0)-(0.00112*data1));
    }
    else
    {
        *lux = 0;
    }
    return 0;
}
