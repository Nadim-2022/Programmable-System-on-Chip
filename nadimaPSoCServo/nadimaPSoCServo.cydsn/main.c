/* ========================================
 *
 * Copyright Nadim Ahmed, Metropolia University of Applied Science, Finland, 2024
 * 
 * April 16, 2024
 *
 * PSoC Design Course 2024: Exercise 2, PWM, Interrupte and Servo
 *
 * Servo motor run  and speed change on button press {4 speed mode Stop, Full speed, Medium speed, Low speed}
 * Button press will change the speed mode
 * 1st press: Full speed
 * 2nd press: Medium speed
 * 3rd press: Low speed
 * 4th press: Stop
 *
 * CC-NA-SA 4.0
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <stdbool.h>

/* Project Defines */
#define MAX_SPEEDS 4
#define SPEED_INCREMENT 10

/* Global variables */
uint8_t speed_index = 0;
uint16_t speed_values[MAX_SPEEDS] = { 1500, 2500, 3500, 4500}; // Stop, Max Speed, Medium speed , Slow Speed
static uint16_t current_speed_value = 0;

//Interrupt service routines
CY_ISR_PROTO(button_press_isr);
CY_ISR_PROTO(pwm_cycle_isr);

int main(void)
{   
    CyGlobalIntEnable; /* Enable global interrupts. */
    Clock_1_Start();
    Button_isr_StartEx(button_press_isr);
    LED1_Write(0);
    PWM_Start();
    PWM_ctrl_StartEx(pwm_cycle_isr);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    for(;;)
    {
        // Main loop
    }
}

/*
* ISR for button press
* This ISR is called when the button is pressed
* It will increment the current speed index
* This will change the target speed
* The PWM ISR will then change the current speed to the target speed
*/
CY_ISR(button_press_isr){
     /* Increment and check speed index */
    if (++speed_index >= MAX_SPEEDS) {
        speed_index = 0;    
    }
    Button_ClearInterrupt();        // Clears the pending interrupt.
}

/*
* ISR for PWM
* This ISR is called when PWM is done with the current cycle
* It will increment or decrement the current speed based on the target speed
* It will then update the PWM compare value to the new speed
* This will create a smooth transition between speeds
* This is done by incrementing or decrementing the current speed by a fixed step
* until it reaches the target speed
* The step size is defined by SPEED_INCREMENT
* The target speed is defined by speed_values[speed_index]
* The current speed is stored in current_speed_value
* The PWM compare value is updated with the new current speed
*/
CY_ISR(pwm_cycle_isr){
    if (PWM_ReadStatusRegister() ) {
        bool isCurrentSpeedLess = current_speed_value < speed_values[speed_index];
        bool isCurrentSpeedMore = current_speed_value > speed_values[speed_index];
        if (isCurrentSpeedLess) {
            current_speed_value += SPEED_INCREMENT;
            if (isCurrentSpeedLess) {
                current_speed_value = speed_values[speed_index];
            }
        } else if (isCurrentSpeedMore) {
            current_speed_value -= SPEED_INCREMENT;
            if (isCurrentSpeedMore) {
                current_speed_value = speed_values[speed_index];
            }
        }
        PWM_WriteCompare(current_speed_value);
    }
}

/* [] END OF FILE */