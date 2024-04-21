/* ========================================
 *
 * Copyright: Nadim Ahmed, Metropolia University of Applied Science, Finland, 2024
 * 
 * April 21, 2024
 *
 * PSoC Design Course 2024: Exercise 5, OneWire
 *
 * One Wire protocol implementation and Temperature reading
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

/* DS18B20 */
uint8_t Rh_byte1, Rh_byte1, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;
float32 Tempreture = 0;
uint8_t Presence = 0;

uint8_t DS18B20_Start();
void DS18B20_Write(uint8_t data);
uint8_t DS18B20_Read();
void DS18B20_tempCalc();
uint8 retByte_buffer[9];

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
    
   
       if(send_data){
            CyGlobalIntDisable;
            Presence = DS18B20_Start();
            CyDelay(1);
            DS18B20_Write(0xCC);
            DS18B20_Write(0x4E);
            DS18B20_Write(0);
            DS18B20_Write(0);
            DS18B20_Write(0);
            
            Presence = DS18B20_Start();
            DS18B20_Write(0xCC);
            DS18B20_Write(0x44);
            OneWire_SetDriveMode(OneWire_DM_STRONG);
            CyDelay(94);                                         
            OneWire_SetDriveMode(OneWire_DM_RES_UP);
           
            CyDelay(800);
            
            Presence = DS18B20_Start();
            CyDelay(1);
            DS18B20_Write(0xCC);
            DS18B20_Write(0xBE);
            
            Temp_byte1 = DS18B20_Read();
            Temp_byte2 = DS18B20_Read();
            
            
            TEMP = Temp_byte2 << 8 | Temp_byte1;
            Tempreture = (float)TEMP /16;
            sprintf(TransmitBuffer, "{ \"ADC\": %d ,\"LM35\": %d, \"DS18B20\": %d}\r\n", (int) voltage, (int)current_tempreture,(int) Tempreture);
            UART_1_PutString(TransmitBuffer);
            send_data = false;
            CyGlobalIntEnable;
        
        }
        
         
    }
}

uint8_t DS18B20_Start(){
    uint8_t Response = 0;
    OneWire_Write(0);
    CyDelayUs(480);
    OneWire_Write(1);
    CyDelayUs(80);
    if(!OneWire_Read()){
        Response = 1;
    }
    CyDelayUs(450);
    return Response;
}
void DS18B20_Write(uint8_t data){
        for(int i = 0; i < 8; i++){
            if((data & 1<<i) != 0 ){
                OneWire_Write(0);
                CyDelayUs(1);
                OneWire_Write(1);
                CyDelayUs(60);
            }
            else{
                OneWire_Write(0);
                CyDelayUs(60);
                OneWire_Write(1);
                CyDelayUs(15);
                
            }
        }
    
}

uint8_t DS18B20_Read(){
    uint8_t value = 0;
    for(int i = 0; i < 8; i++){
        OneWire_Write(0);
        CyDelayUs(2);
        OneWire_Write(1);
        CyDelayUs(10);
        if(OneWire_Read()){
            value |= 1<<i;
        }
        CyDelayUs(50);
    }                           
    return value;  
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
