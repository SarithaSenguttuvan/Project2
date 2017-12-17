/**
 * @file
 *      main.c       *BBG*
 * @brief
 *      This file contains functionalities common to all the tasks
 * @author
 *      Saritha Senguttuvan, Savitha Senguttuvan
 * @date
 *      December 12,2017
 *
 */

#include "genericTiva.h"
#include "main.h"
#include "mainTiva.h"
#include "socketTiva.h"
#include "lightTiva.h"
#include "accelerometerTiva.h"
#include "I2C_LightTiva.h"
#include "sckt.h"

/* Handles for the tasks create by main(). */
TaskHandle_t xMainTaskHandle = NULL;
TaskHandle_t xSocketTaskHandle = NULL;
TaskHandle_t xLightTaskHandle = NULL;
TaskHandle_t xAccelerometerTaskHandle = NULL;
TaskHandle_t xScktHandle = NULL;
//TaskHandle_t xI2cLightTaskHandle = NULL;

uint32_t output_clock_rate_hz;

// Main function
int main(void)
{
    // Initialize system clock to 120 MHz

    BaseType_t xStatus;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    UARTStdioConfig(0, 115200, SYSTEM_CLOCK);

    UARTprintf("\r\n Starting the execution");

    // Create tasks
#if 0
    xStatus = xTaskCreate(clientSckt_task, (const portCHAR *)"sckt", configMINIMAL_STACK_SIZE, NULL, 3, &xScktHandle);
    if(xStatus != pdPASS)
        return 0;
#endif

#if 1
    xStatus = xTaskCreate(lightTask, (const portCHAR *)"light", configMINIMAL_STACK_SIZE, NULL, 6, &xLightTaskHandle);
    if(xStatus != pdPASS)
        return 0;

    xStatus = xTaskCreate(accelerometerTask, (const portCHAR *)"accelerometer", configMINIMAL_STACK_SIZE, NULL, 5, &xAccelerometerTaskHandle);
    if(xStatus != pdPASS)
        return 0;

    xStatus = xTaskCreate(socketTask, (const portCHAR *)"socket", configMINIMAL_STACK_SIZE, NULL, 4, &xSocketTaskHandle);
    if(xStatus != pdPASS)
        return 0;

    xStatus = xTaskCreate(mainTask, (const portCHAR *)"main", configMINIMAL_STACK_SIZE, NULL, 3, &xMainTaskHandle);
    if(xStatus != pdPASS)
        return 0;
#endif
    //vTaskDelay(1000);
    vTaskStartScheduler();

    return 0;
}

#if 0
// Flash the LEDs on the launchpad
void demoLEDTask(void *pvParameters)
{
    for (;;)
    {
        // Turn on LED 1
        LEDWrite(0x0F, 0x01);
        vTaskDelay(1000);

        // Turn on LED 2
        LEDWrite(0x0F, 0x02);
        vTaskDelay(1000);

        // Turn on LED 3
        LEDWrite(0x0F, 0x04);
        vTaskDelay(1000);

        // Turn on LED 4
        LEDWrite(0x0F, 0x08);
        vTaskDelay(1000);
    }
}


// Write text over the Stellaris debug interface UART port
void demoSerialTask(void *pvParameters)
{
    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 57600, SYSTEM_CLOCK);


    for (;;)
    {
        UARTprintf("\r\nHello, world from FreeRTOS 9.0!");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
#endif
/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {

    }
}
