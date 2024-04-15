/* ========================================
 *
 * Copyright Nadim Ahmed, Metropolia University of Applied Science, Finland, 2024
 * 
 * April 4, 2024
 *
 * PSoC Design Course 2024: Exercise 1, Fundamentals
 *
 * Blink LED at btton press 1/sec ( 200ms on / 800ms off) and send counter value through UART to terminal
 *
 * CC-NA-SA 4.0
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>

int main(void)
{   
    // Counter variable
    uint8 blnkcounter = 0;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    // Start UART
    UART_Start();
    
    // Buffer to  send name
    char name[20] = {"Nadim Ahmed\r\n"};
    
    // Buffer to send counter value
    char send[20] = {0};

    UART_PutString(name);
    for(;;)
    {
        /* Place your application code here. */
        
        if(!Button_Read()){  // if Button pressed
            uint8 ledState = ++blnkcounter % 2;
            LED1_Write(ledState); // Increase the counter and turn on and off the led
            CyDelay(ledState == 0 ? 200 : 800); // if led on wait 200 ms, else wait 800 ms
        }
        // Read input  char from UART and send the blnk counter value
        if (UART_GetChar()) {
            sprintf(send, "%d\r\n", blnkcounter);
            UART_PutString(send);
        }
    }
}

/* [] END OF FILE */
