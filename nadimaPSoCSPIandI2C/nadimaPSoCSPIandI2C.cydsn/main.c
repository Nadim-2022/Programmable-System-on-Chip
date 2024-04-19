/* ========================================
 *
 * Copyright: Nadim Ahmed, Metropolia University of Applied Science, Finland, 2024
 * 
 * April 18, 2024
 *
 * PSoC Design Course 2024: Exercise 4, SPI & I2C
 *
 * Data acquisition from temperature sensor using SPI and I2C, 
 * send the temperature through Uart and twice in a second
 *
 * CC-NA-SA 4.0
 *
 * ========================================
*/

#include <project.h>
#include "stdio.h"
#include "stdbool.h"
/* Project Defines */
#define Data_Buffer_SIZE 64
#define TC74Address 0b1001010

/* Global variables */
bool isDataReady = false;   
uint32_t voltageSum = 0;
uint32_t sampleCount = 0;
uint32_t voltageSumIsr = 0;
uint32_t sampleCountIsr = 0;
float32 temperature = 0;
float32 averageVoltage;
uint16 TC74Value = 0;
uint16 SPIMValue = 0;

/* Function declarations */
void calculateTemperature();
void handleSPI();
uint8_t handleI2C();
void sendDataToUART();

/* Interrupt declarations */
CY_ISR_PROTO(ADCInterruptHandler);
CY_ISR(TimerInterruptHandler);

int main()
{
    /* Enable global interrupts */
    CyGlobalIntEnable; 


    /* Start the components and interrupts */
    UART_1_Start();
    SPIM_1_Start();
    I2C_1_Start();
    ADC_DelSig_1_Start();
    Timer_Start();
    
    ADC_isr_StartEx(ADCInterruptHandler);
    Timer_isr_StartEx(TimerInterruptHandler);

    /* Send message to verify COM port is connected properly */
    UART_1_PutString("COM Port Open\r\n");    
    
    /* Start the ADC conversion */
    ADC_DelSig_1_StartConvert();

    for(;;)
    {   
         if (isDataReady) {
            handleSPI();
            TC74Value = handleI2C();
            calculateTemperature();
            sendDataToUART();
            isDataReady = false;  
        }
    }
}


/* Function to handle SPI data */
void handleSPI() {
    SPIM_1_WriteTxData(SPIMValue);
    SPIMValue = SPIM_1_ReadRxData();
    SPIMValue = (SPIMValue >> 1) & 0x07FF; 
}

/* Function to handle I2C communication */
uint8_t handleI2C() {
    I2C_1_MasterSendStart(TC74Address, I2C_1_WRITE_XFER_MODE);
    I2C_1_MasterWriteByte(0x00);
    I2C_1_MasterSendRestart(TC74Address, I2C_1_READ_XFER_MODE);      
    TC74Value = I2C_1_MasterReadByte(I2C_1_NAK_DATA);     
    I2C_1_MasterSendStop();
    return TC74Value;
}

/* Function to send data to UART */
void sendDataToUART() {
    /* Format data */ 
    char Data_Buffer[Data_Buffer_SIZE];
    sprintf(Data_Buffer, 
        "{ \"ADC\": %d , \"LM35\": %d, \"TC74\": %d, \"SPI\": %d }\r\n", 
        (int)averageVoltage, (int)temperature, TC74Value, SPIMValue/10 );     
    /* Send data to UART */
    UART_1_PutString(Data_Buffer);
}

/* Function to calculate temperature */
void calculateTemperature() {
    averageVoltage = voltageSumIsr / sampleCountIsr;    
    temperature = averageVoltage / 10; 
}

/* ADC Interrupt Service Routine */
CY_ISR(ADCInterruptHandler) {
    uint32_t adcOutput = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult16());
    voltageSum += adcOutput;
    sampleCount++;
}

/* Timer Interrupt Service Routine */
CY_ISR(TimerInterruptHandler) {
    Timer_ReadStatusRegister();
    ADC_isr_Disable();
    voltageSumIsr = voltageSum;
    sampleCountIsr = sampleCount;
    voltageSum = 0;
    sampleCount = 0;
    ADC_isr_Enable();
    isDataReady = true;
}
