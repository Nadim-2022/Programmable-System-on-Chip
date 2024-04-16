/*******************************************************************************
* File Name: PWM_ctrl.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_PWM_ctrl_H)
#define CY_ISR_PWM_ctrl_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void PWM_ctrl_Start(void);
void PWM_ctrl_StartEx(cyisraddress address);
void PWM_ctrl_Stop(void);

CY_ISR_PROTO(PWM_ctrl_Interrupt);

void PWM_ctrl_SetVector(cyisraddress address);
cyisraddress PWM_ctrl_GetVector(void);

void PWM_ctrl_SetPriority(uint8 priority);
uint8 PWM_ctrl_GetPriority(void);

void PWM_ctrl_Enable(void);
uint8 PWM_ctrl_GetState(void);
void PWM_ctrl_Disable(void);

void PWM_ctrl_SetPending(void);
void PWM_ctrl_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the PWM_ctrl ISR. */
#define PWM_ctrl_INTC_VECTOR            ((reg32 *) PWM_ctrl__INTC_VECT)

/* Address of the PWM_ctrl ISR priority. */
#define PWM_ctrl_INTC_PRIOR             ((reg8 *) PWM_ctrl__INTC_PRIOR_REG)

/* Priority of the PWM_ctrl interrupt. */
#define PWM_ctrl_INTC_PRIOR_NUMBER      PWM_ctrl__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable PWM_ctrl interrupt. */
#define PWM_ctrl_INTC_SET_EN            ((reg32 *) PWM_ctrl__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the PWM_ctrl interrupt. */
#define PWM_ctrl_INTC_CLR_EN            ((reg32 *) PWM_ctrl__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the PWM_ctrl interrupt state to pending. */
#define PWM_ctrl_INTC_SET_PD            ((reg32 *) PWM_ctrl__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the PWM_ctrl interrupt. */
#define PWM_ctrl_INTC_CLR_PD            ((reg32 *) PWM_ctrl__INTC_CLR_PD_REG)


#endif /* CY_ISR_PWM_ctrl_H */


/* [] END OF FILE */
