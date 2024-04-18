/* ========================================
 *
 * Copyright: Nadim Ahmed, Metropolia University of Applied Science, Finland, 2024
 * 
 * April 17, 2024
 *
 * PSoC Design Course 2024: Exercise 3, DAQ
 *
 *Data acquisition from  temperature sensor
 *
 * CC-NA-SA 4.0
 *
 * ========================================
*/

#include <project.h>
#include "stdio.h"
#include "stdbool.h"

/* Project Defines */
#define TRANSMIT_BUFFER_SIZE  30

CY_ISR_PROTO(ADC_ISR_Function);
CY_ISR(Timer_ISR_Function);

void send_Data_to_Uart();
void tempcalc();

uint32 sum_of_voltage = 0 ;
uint32 count = 0;

float32 voltage;
float32 current_tempreture;
bool send_data = false;
char TransmitBuffer[TRANSMIT_BUFFER_SIZE];

int main()
{
   CyGlobalIntEnable;
    /* Start the components */
    ADC_DelSig_1_Start();
    UART_1_Start();
    Timer_Start();
  
    /* Start interrupt */
    ADC_isr_StartEx(ADC_ISR_Function);
    Timer_isr_StartEx(Timer_ISR_Function);
    
    /* Start the ADC conversion */
    ADC_DelSig_1_StartConvert();
    
    /* Send message to verify COM port is connected properly */
    UART_1_PutString("COM Port Open");
    
    for(;;)
    {    
       /* Send data through UART */ 
       if(send_data){
        sprintf(TransmitBuffer, "{ \"ADC\": %d ,\"LM35\": %.1f}\r\n", (int) voltage, current_tempreture);
        UART_1_PutString(TransmitBuffer);
        send_data = false;
    }
         
    }
}
/* ADC ISR */
CY_ISR(ADC_ISR_Function){
   uint32 current_voltage =  ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult16());
   sum_of_voltage += current_voltage;
   count++;
}
/* Timer ISR */
CY_ISR(Timer_ISR_Function){
    Timer_ReadStatusRegister();
    ADC_isr_Disable();
    voltage = sum_of_voltage / count;
    current_tempreture = voltage / 10.0f;
    sum_of_voltage = 0;
    count = 0;
    ADC_isr_Enable();
    send_data = true;
}

/* [] END OF FILE */
