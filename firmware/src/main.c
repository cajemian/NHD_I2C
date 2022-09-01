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

//Flags to indicate pass/fail of system checks before Cleaning Cycle
typedef struct
{
    uint8_t podMissing;
    uint8_t podExhausted;
    uint8_t caseMissing;
    uint8_t cathMissing;
    uint8_t cathExhausted;
    uint8_t leaking;
    uint8_t wasteWaterReservoir;

} SYSTEMS_CHECK_FLAGS;

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
    DISPLAY_STATES  previousState = CLEANING;           //TODO Going back to beginning of state or middle?
    SYSTEMS_CHECK_FLAGS sysCheckFlgs = {0, 0, 0, 0, 0, 0, 0};   // Init all flags to 0
    int timeofstate = 0;
    
    HRTBEAT_LED_Set();
    
    I2C_Reset_Set();        // Keep Reset LED high
    
    int selFlg = 0;
    int nav1Flg = 0;
    int nav2Flg = 0;
    int ejectFlg = 0;
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
             ejectFlg = !ejectFlg;
             //stateMachine = ABC;
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
            switch(stateMachine){           //TODO Can all buttons/time be cleared between state changes?
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
                        stateMachine = SYSTEMSCHECKS;   //  Move on to System Checks
                        timeofstate = 0;                //  Reset time of state
                        selFlg = 0;
                    }
                    break;
                case MENU:
                    if(nav1Flg == 1){
                        stateMachine = RUNCYCLE;
                        nav1Flg = 0;
                    }
                    if(selFlg == 1){
                        //stateMachine = SYSTEMSCHECKS;       // TODO Move on to Menu
                        timeofstate = 0;                      
                        selFlg = 0;
                    }
                    break;
                case SYSTEMSCHECKS:
                    if(sysCheckFlgs.podMissing == 1){
                        stateMachine = PODMISSING;
                    }
                    else if(sysCheckFlgs.podExhausted == 1){
                        stateMachine = PODEXHAUSTED;
                    }
                    else if(sysCheckFlgs.caseMissing == 1){
                        stateMachine = CASEMISSING;
                    }
                    else if(sysCheckFlgs.cathMissing == 1){
                        stateMachine = CATHMISSING;
                    }
                    else if(sysCheckFlgs.cathExhausted == 1){
                        stateMachine = CATHEXHAUSTED;
                    }
                    else if(sysCheckFlgs.leaking == 1){
                        stateMachine = LEAKING;
                    }
                    else if(sysCheckFlgs.wasteWaterReservoir == 1){
                        stateMachine = WASTEWATERRESERVOIR;
                    }
                    else{
                        if(timeofstate >= 5){
                            stateMachine = CLEANING;
                            timeofstate = 0;
                        }
                    }
                    
                    break;
                case PODMISSING:
                    //Press Select to Continue
                    if(selFlg == 1){
                        stateMachine = RUNCYCLE;        // Back to Idle Screen
                        selFlg = 0;     // Reset Select flag
                    }
                    break;
                case PODEXHAUSTED:
                    if(selFlg == 1){
                        stateMachine = RUNCYCLE;        // Back to Idle Screen
                        selFlg = 0;     // Reset Select flag
                    }
                    break;
                case CASEMISSING:
                    if(selFlg == 1){
                        stateMachine = RUNCYCLE;        // Back to Idle Screen
                        selFlg = 0;     // Reset Select flag
                    }
                    break;
                case CATHMISSING:
                    if(selFlg == 1){
                        stateMachine = RUNCYCLE;        // Back to Idle Screen
                        selFlg = 0;     // Reset Select flag
                    }
                    break;
                case CATHEXHAUSTED:
                    if(selFlg == 1){
                        stateMachine = RUNCYCLE;        // Back to Idle Screen
                        selFlg = 0;     // Reset Select flag
                    }
                    break;
                case LEAKING:
                    if(selFlg == 1){
                        stateMachine = RUNCYCLE;        // Back to Idle Screen
                        selFlg = 0;     // Reset Select flag
                    }
                    break;
                case WASTEWATERRESERVOIR:
                    if(selFlg == 1){
                        stateMachine = RUNCYCLE;        // Back to Idle Screen
                        selFlg = 0;     // Reset Select flag
                    }
                    break;
                case CLEANING:
                    if(selFlg == 1){
                        stateMachine = CANCELSEL0;
                        previousState = CLEANING;
                        selFlg = 0;
                        nav1Flg = 0;
                        nav2Flg = 0;
                    }
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = DISINFECTION;
                        timeofstate = 0;
                    }
                    break;
                case DISINFECTION:
                    if(selFlg == 1){
                        stateMachine = CANCELSEL0;
                        previousState = DISINFECTION;
                        selFlg = 0;
                        nav1Flg = 0;
                        nav2Flg = 0;
                    }
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = DRYING;
                        timeofstate = 0;
                    }
                    break;
                case DRYING:
                    if(selFlg == 1){
                        stateMachine = CANCELSEL0;
                        previousState = DRYING;
                        selFlg = 0;
                        nav1Flg = 0;
                        nav2Flg = 0;
                    }
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = COMPLETE;
                        timeofstate = 0;
                    }
                    break;
                case CANCELSEL0:
                    if(selFlg == 1){
                        // Continue Cycle
                        stateMachine = previousState;   //Go back to state before cancel
                        selFlg = 0;
                        timeofstate = 0;
                    }
                    if(nav1Flg == 1 || nav2Flg == 1){   //Navigate to select other option
                        stateMachine = CANCELSEL1;      // Highlight Cancel Cycle
                        nav1Flg = 0;
                        nav2Flg = 0;
                    }
                    break;
                case CANCELSEL1:
                    if(selFlg == 1){
                        stateMachine = CANCELLED;
                        timeofstate = 0;
                    }
                    if(nav1Flg == 1 || nav2Flg == 1){   //Navigate to select other option
                        stateMachine = CANCELSEL0;      // Highlight Continue Cycle
                        nav1Flg = 0;
                        nav2Flg = 0;
                    }
                    break;
                case COMPLETE:
                    if(ejectFlg == 1){
                        // Use Catheter, Lubrication
                    }
                    break;
                case CANCELLED:
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = PURGE;
                        timeofstate = 0;
                    }
                    break;
                case PURGE:
                    break;
                case LUBRICATION:
                    if(selFlg == 1){
                        //cancel cycle
                    }
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        timeofstate = 0;
                    }
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

