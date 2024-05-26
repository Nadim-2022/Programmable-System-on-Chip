/* ========================================
 *
 * Copyright: Nadim Ahmed, Metropolia University of Applied Science, Finland, 2024
 * 
 * May 26, 2024
 *
 * PSoC Design Course 2024: Data Acquisition add-on for Raspberry Pi Pico W
 *
 * PSoC device acts as an I2C slave to acquire analog sensor data and convert it to temperature readings.
 *
 * The temperature readings are transmitted to the I2C master.
 * 
 * CC-NA-SA 4.0
 *
 * ========================================
*/


#include "project.h"
#include "stdio.h"
#include "stdbool.h"

#define databuffer 10
#define debugebuffer 256
#define i2cbuffer 4

// Can be define only one of "send_last_reading" or "fullbuffer"
#define send_last_reading
//#define fullbuffer

bool isDataReady = false;

uint32 adc1;
uint32 adc2;
uint32 adc3;
uint32 adc4;
uint32 adcCount;

uint32 TotalADC1;
uint32 TotalADC2;
uint32 TotalADC3;
uint32 TotalADC4;
uint32 adcCount2;

uint8 temp1;
uint8 temp2;
uint8 temp3;
uint8 temp4;


    
CY_ISR_PROTO(ADCInterruptHandler);
CY_ISR(TimerInterruptHandler);

void calculateTemp();

// Can be send the complete buffer if want 4 x 10
struct
{
uint8 data0[databuffer];
uint8 data1[databuffer];
uint8 data2[databuffer];
uint8 data3[databuffer];
} __attribute__ ((packed)) ezi2cBuffer;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    char transmit_buffer[debugebuffer];
    uint8 i2cbuf[i2cbuffer] = {0,0,0,0};
    int count = 0;
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_Start();
    ADC_SAR_Seq_1_Start();
    Timer_Start();
    EZI2C_Start();
    memset(&ezi2cBuffer, 0, sizeof(ezi2cBuffer));
    
    //if want to send only last 4 data
#ifdef send_last_reading
    EZI2C_SetBuffer1(i2cbuffer,0,i2cbuf);
#endif
    
    // if want to send full buffer
#ifdef fullbuffer
    EZI2C_SetBuffer1(sizeof(ezi2cBuffer),0,(uint8 *)&ezi2cBuffer);
#endif
    
    ADC_SAR_Seq_1_StartConvert();
    ADC_ISR_StartEx(ADCInterruptHandler);
    Timer_ISR_StartEx(TimerInterruptHandler);
   
    UART_1_PutString("COM Port Open\r\n");
    for(;;)
    {   
        /* Place your application code here. */
        
        // send only last temperature value to I2C master for 4 sensors
#ifdef send_last_reading
        if(isDataReady){
            isDataReady = false;
            calculateTemp();
            sprintf(transmit_buffer,"Temp1: %d C Temp2: %d C Temp3: %d C Temp4: %d C\r\n", 
                temp1, temp2, temp3, temp4);
            UART_1_PutString(transmit_buffer);
          
            ezi2cBuffer.data0[count]=temp1;
            ezi2cBuffer.data1[count]=temp2;
            ezi2cBuffer.data2[count]=temp3;
            ezi2cBuffer.data3[count]=temp4;
     
            i2cbuf[0] = ezi2cBuffer.data0[count];
            i2cbuf[1] = ezi2cBuffer.data1[count];
            i2cbuf[2] = ezi2cBuffer.data2[count];
            i2cbuf[3] = ezi2cBuffer.data3[count];
            count++;
            if ( count > 10) count = 0;
#endif    
        // send full buffer temperature value to I2C master for 4 sensors 4 x 10
#ifdef fullbuffer
        if(isDataReady){
            isDataReady = false;
            calculateTemp();
            sprintf(transmit_buffer,"Temp1: %d C Temp2: %d C Temp3: %d C Temp4: %d C\r\n", 
                temp1, temp2, temp3, temp4);
            UART_1_PutString(transmit_buffer);
            
            ezi2cBuffer.data0[count]=temp1;
            ezi2cBuffer.data1[count]=temp2;
            ezi2cBuffer.data2[count]=temp3;
            ezi2cBuffer.data3[count]=temp4;
            
             if (count == 10){
                // Shift the buffer to remove the oldest value, send last 10 values
                memcpy(ezi2cBuffer.data0, &ezi2cBuffer.data0[1], (10 ) * sizeof(uint8_t));
                memcpy(ezi2cBuffer.data1, &ezi2cBuffer.data1[1], (10 ) * sizeof(uint8_t));
                memcpy(ezi2cBuffer.data2, &ezi2cBuffer.data2[1], (10 ) * sizeof(uint8_t));
                memcpy(ezi2cBuffer.data3, &ezi2cBuffer.data3[1], (10 ) * sizeof(uint8_t));
            }
            
            count++;
            if ( count > 10) count = 10;
#endif
            
        }
        
    }
}

// Calculates the tempreture for 4 sensor
void calculateTemp(){
    temp1 = (TotalADC1/adcCount2)/10;
    temp2 = (TotalADC2/adcCount2)/10;
    temp3 = (TotalADC3/adcCount2)/10;
    temp4 = (TotalADC4/adcCount2)/10;
}

// Collect the analog sensor data 25000 SPS ADC Interrupt Service Routine

CY_ISR(ADCInterruptHandler) {
    adc1 += ADC_SAR_Seq_1_CountsTo_mVolts(ADC_SAR_Seq_1_GetResult16(0));
    adc2 += ADC_SAR_Seq_1_CountsTo_mVolts(ADC_SAR_Seq_1_GetResult16(1));
    adc3 += ADC_SAR_Seq_1_CountsTo_mVolts(ADC_SAR_Seq_1_GetResult16(2));
    adc4 += ADC_SAR_Seq_1_CountsTo_mVolts(ADC_SAR_Seq_1_GetResult16(3));
    adcCount++;
    
}


// Timer Interrupt Service Routine Down sample 10 times in a second
CY_ISR(TimerInterruptHandler) {
    Timer_ReadStatusRegister();
    ADC_ISR_Disable();
    TotalADC1 = adc1;
    TotalADC2 = adc2;
    TotalADC3 = adc3;
    TotalADC4 = adc4;
    adcCount2 = adcCount;
    adc1 = 0;
    adc2 = 0;
    adc3 = 0;
    adc4 = 0;
    adcCount = 0;
    ADC_ISR_Enable();
    isDataReady = true;
}


/* [] END OF FILE */
