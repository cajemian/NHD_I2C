/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <string.h>
#include "NHD-2.23-12832UCxx.h"
#include "graphics.h"

uint8_t drawLine = 0;
uint8_t pageAddress = 0;        //Page 0 out of 4
uint8_t colAddress = 4;         //Global address of column pixel, Start with 4


//10ms Interrupt
void TC3_Callback_InterruptHandler(TC_TIMER_STATUS status, uintptr_t context)
{   //TODO Change to 10ms and toggle Valves
    /* Toggle LED, Visible Tick */     
    counter10ms++; 
    //LED_YEL_Toggle();
    LED_BLU_Toggle();
    LED_GRN_Toggle();
    LED_RED_Toggle();
    
}

int main ( void )
{
   
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    //Start Interrupt
    TC3_TimerCallbackRegister(TC3_Callback_InterruptHandler, (uintptr_t)NULL);
    
    TC3_TimerStart();
    
    counter10ms = 0;
    counter100ms = 0;
    counter1S = 0;
    DISPLAY_STATES  stateMachine = SETUP;
    int timeofstate = 0;
    
    HRTBEAT_LED_Set();
    
    I2C_Reset_Set();        // Keep Reset LED high
    
    int selFlg = 0;
    int nav1Flg = 0;
    int nav2Flg = 0;
    stateMachineLoop(stateMachine);    // Setup Display before writing to it
     
    while(true)
    {   
         /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        printf("counter10ms: %d\r\n", counter10ms);
        printf("counter100ms: %d\r\n", counter100ms);
        printf("counter1s: %d\r\n\r\n", counter1S);

        if(CASE_MISS_Get() == 1){
            //printf("CASE MISSING Toggle LED");
            LED_YEL_Toggle();
            selFlg = !selFlg;
            if(selFlg == 1){
                //state = SYSTEMSCHECKS;
            }else{
            }
        }
        
        if(EJECT_BTN_Get() == 1){
             HRTBEAT_LED_Toggle();
             stateMachine = ABC;
        }
        
        if(SELECT_BTN_Get() == 1){
             HRTBEAT_LED_Toggle();
             selFlg = !selFlg;
        }
        
        if(POWER_BTN_Get() == 1){
             stateMachine = POWERUP;
             timeofstate = 0;
             
        }
        
        if(NAV_BTN_A_Get() == 1){
             LED_YEL_Toggle();
             nav1Flg = !nav1Flg;
        }
        
        if(NAV_BTN_B_Get() == 1){
             LED_YEL_Toggle();
             nav2Flg = !nav2Flg;
        }
            
        if(counter10ms >= 1){
            //printf("10ms");
            counter10ms = 0;
            counter100ms++;        //count to 1S    
        }
        
        if(counter100ms >= 10){
            
            counter100ms = 0;
            counter1S++;        //count to 1S   
        }
        
        if(counter1S >= 10){
            switch(stateMachine){
                case SETUP:
                    break;
                case CLEAR:
                    break;
                case ABC:
                    if(timeofstate >= 5){
                        stateMachine = RUNCYCLE;
                    }
                    break;
                case POWERUP:
                    if(timeofstate >= 3){
                        stateMachine = RUNCYCLE;
                    }
                    break;
                
                case RUNCYCLE:
                    if(nav1Flg == 1){
                        stateMachine = MENU;
                        nav1Flg = 0;
                    }
                    if(selFlg == 1){
                        stateMachine = SYSTEMSCHECKS;
                    }
                    break;
                case MENU:
                    if(nav1Flg == 1){
                        stateMachine = RUNCYCLE;
                        nav1Flg = 0;
                    }
                    if(selFlg == 1){
                        stateMachine = SYSTEMSCHECKS;
                    }
                    break;
                case SYSTEMSCHECKS:
                    break;
                default:
                    break;
                            
            }
            
            stateMachineLoop(stateMachine);  //Update screen based off state
            counter1S = 0;
            timeofstate++;
        }

    } 
    

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

