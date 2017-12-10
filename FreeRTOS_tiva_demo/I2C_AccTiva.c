/*
 * I2C_LightTiva.c
 *
 *  Created on: Dec 2, 2017
 *      Author: Saritha
 */

#include "genericTiva.h"
#include "I2C_AccTiva.h"

extern uint32_t output_clock_rate_hz;

/* I2C setup function */
int8_t i2cAccSetup()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);         /* Enable the I2C0 peripheral */
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C3));  /* Wait for the I2C0 module to be ready */

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);        /* Port B */
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));

    GPIOPinConfigure(GPIO_PK4_I2C3SCL);                 /* PB2 SCL */
    GPIOPinConfigure(GPIO_PK5_I2C3SDA);                 /* PB2 SDA */

    GPIOPinTypeI2CSCL(GPIO_PORTK_BASE, GPIO_PIN_4);
    GPIOPinTypeI2C(GPIO_PORTK_BASE, GPIO_PIN_5);

    I2CMasterInitExpClk(I2C3_BASE, output_clock_rate_hz, false);     /* Initialize Master and Slave */
    return 0;
}

int8_t write_control1_reg_acc(uint8_t value)
{
    int8_t reg_addr = I2C_ACC_CONTROL_REG1 | I2C_CMD_CODE_ACC;       /* Reg address with command code */
    write_reg_acc(reg_addr, value);     /* Write to the register */
    return 0;
}
/* Write to the register */
int8_t write_reg_acc(int8_t write_reg_addr, int8_t reg_value)        /* Function to write the value into the Register address */
{
    uint32_t err_check = 0;
    I2CMasterSlaveAddrSet(I2C3_BASE, I2C_ACC_SLAVE_ADDR, false);  /* Specify slave address */
    I2CMasterDataPut(I2C3_BASE, write_reg_addr);           /* Register address */
    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);    /* Initiate send of character from Master to Slave */
    while (!(I2CMasterBusy(I2C3_BASE))); /* Wait till end of transaction */
    while(I2CMasterBusy(I2C3_BASE));  /* Delay until transmission completes */

    err_check = I2CMasterErr(I2C3_BASE);
    if((err_check == I2C_MASTER_ERR_NONE))
    {
    }
    else
    {
    }

    /* Data */
    I2CMasterDataPut(I2C3_BASE, reg_value);           /* Register value */
    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);    /* Initiate send of character from Master to Slave */
    while (!(I2CMasterBusy(I2C3_BASE))); /* Wait till end of transaction */
    while(I2CMasterBusy(I2C3_BASE));  /* Delay until transmission completes */

    err_check = I2CMasterErr(I2C3_BASE);
    if((err_check == I2C_MASTER_ERR_NONE))
    {
    }
    else
    {
    }
    return 0;
}


/* Read data from the register address */
int8_t read_data_acc(int8_t reg_addr, uint8_t *data_val, uint8_t num_bytes)
{
    uint8_t i = 0;
    uint32_t err_check = 0;
    *data_val = 0;
    int8_t read_reg_addr = reg_addr | I2C_CMD_CODE;             /* register addres with the command code */

    /* Writing the Address */
    I2CMasterSlaveAddrSet(I2C3_BASE, I2C_ACC_SLAVE_ADDR, false);  /* Specify slave address */

    I2CMasterDataPut(I2C3_BASE, read_reg_addr);                     /* Register address */

    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);        /* Initiate send of character from Master to Slave */

    while (!(I2CMasterBusy(I2C3_BASE)));                            /* Wait till end of transaction */
    while(I2CMasterBusy(I2C3_BASE));                                /* Delay until transmission completes */
    err_check = I2CMasterErr(I2C3_BASE);
    if((err_check == I2C_MASTER_ERR_NONE))
    {
    }
    else
    {
    }
    /* Reading from the address */
    I2CMasterSlaveAddrSet (I2C3_BASE, I2C_ACC_SLAVE_ADDR, true);  /* set slave address and read */
    while(i<num_bytes)
    {
        if(num_bytes == 1)                  /* if only byte to be read */
            I2CMasterControl (I2C3_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        else if(i == 0)                     /* If first byte of two bytes */
            I2CMasterControl (I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
        else if(i == (num_bytes-1))         /* If last byte of two bytes */
            I2CMasterControl (I2C3_BASE,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

        while (!(I2CMasterBusy(I2C3_BASE)));                            /* Wait till end of transaction */
        while(I2CMasterBusy(I2C3_BASE));                                /* Delay until transmission completes */
        err_check = I2CMasterErr(I2C3_BASE);
        if((err_check == I2C_MASTER_ERR_NONE))
        {
        }
        else
        {
        }

        *(data_val+i) = (uint8_t)I2CMasterDataGet(I2C3_BASE);
        err_check = I2CMasterErr(I2C3_BASE);
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
