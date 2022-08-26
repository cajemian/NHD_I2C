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


static uint8_t setupData[SETUP_COMMANDS] = {
    setDisplayClock, RatioFrequency, setMultiplexRation, setMultiplexRation2, setDisplayOffset, setDisplayOffset2,
    setStartLine, setMasterConfig, setMasterConfig2, setAreaColor, setAreaColor2, setAddressingMode1, setAddressingMode2, setSegmentRemap,
    setComRemap, setComConfig, setComConfig1, setLUT, setLUT1, setLUT1, setLUT1, setLUT1,
    setContrast, setContrast2, setBrightness, setBrightness2, setPrechargePeriod, setPrechargePeriod2, setVCOMH, setVCOMH2, setEntireDisplay, setInverseDisplay, setDisplayON
};

typedef enum
{
    SETUP,
    CLEAR,
    DRAW,
    POWERUP,
    LETTER,
    ABC,
    STOP,

} DISPLAY_STATES;

uint8_t drawLine = 0;
uint8_t pageAddress = 0;        //Page 0 out of 4
uint8_t colAddress = 4;         //Global address of column pixel, Start with 4


//10ms Interrupt
void TC3_Callback_InterruptHandler(TC_TIMER_STATUS status, uintptr_t context)
{   //TODO Change to 10ms and toggle Valves
    /* Toggle LED, Visible Tick */     
    counter10ms++; 
    //LED_YEL_Toggle();
    HRTBEAT_LED_Toggle();


}

int main ( void )
{
   
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    //Start Interrupt
    TC3_TimerCallbackRegister(TC3_Callback_InterruptHandler, (uintptr_t)NULL);
    
    TC3_TimerStart();
    
    DISPLAY_STATES  stateMachine = SETUP;
    
    counter10ms = 0;
    counter100ms = 0;
    counter1S = 0;
    
    HRTBEAT_LED_Set();
    
    Reset_Set();
    
    I2C_Reset_Set();
    
    while(true)
    {   
         /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
         //printf("get input from pin\r\n");
        //printf("%ld", CASE_MISS_Get());
        if(CASE_MISS_Get() == 1){
            //printf("CASE MISSING Toggle LED");
            LED_YEL_Toggle();
            stateMachine++;
            printf("State == %d\r\n", stateMachine);
        }
         
        if(counter10ms >= 1){
            //printf("10ms");
            counter10ms = 0;
            counter100ms++;        //count to 1S    
        }
        
        if(counter100ms >= 10){
            printf("100ms");
            counter100ms = 0;
            counter1S++;        //count to 1S   
        }
        
        if(counter1S >= 10){

        }

            switch(stateMachine){
                case SETUP:
                    for(int i = 0; i < SETUP_COMMANDS; i++){
                        sendCommand(setupData[i]);
                    }
                    stateMachine = CLEAR;
                break;

                case CLEAR: //  TODO Cannot figure out how to clear screen after written to it
                    setPageAddress(0, 3);
                    setColumnAddress(4, 131);
                    setAddressingMode(0x00);
                    ssd1306_SetCursor(0,1);
                    ssd1306_Fill();
                    writeData();


                    //stateMachine = DRAW;
                break;

                case DRAW:
                    setPageAddress(0, 3);
                    setColumnAddress(4, 131);
                    setAddressingMode(0x00);
                    ssd1306_SetCursor(20,10);
                    ssd1306_Fill();
                    //char ABCs[] = "ABCDEFGHIJKL";
                    
                    drawString(off);

                    writeData();
                    //stateMachine = POWERUP;
                    break;
                case POWERUP:
                    setPageAddress(0, 3);
                    setColumnAddress(4, 131);
                    setAddressingMode(0x00);
                    ssd1306_Fill();
                    
                    ssd1306_SetCursor(0,10);
                    //char ABCs2[] = "MNOPQRSTUVW";
                    drawString(pwrUp);

                    writeData();
                    //stateMachine = LETTER;
                    break;
                case LETTER:
                    setPageAddress(0, 3);
                    setColumnAddress(4, 131);
                    setAddressingMode(0x00);
                    ssd1306_SetCursor(0,1);
                    ssd1306_Fill();
                    
                    ssd1306_SetCursor(0,19);
                    drawString(nums);

                    writeData();
                    //stateMachine = ABC;
                    break;
                case ABC:
                    setPageAddress(0, 3);
                    setColumnAddress(4, 131);
                    setAddressingMode(0x00);
                    
                    ssd1306_SetCursor(30,19);
                    ssd1306_Fill();
                    drawString(ABCs);

                    writeData();
                    //stateMachine = STOP;
                    break;
                case STOP:
                    stateMachine = CLEAR;
                    break;
                default:
                    stateMachine = CLEAR;
                    break;
            }
        
        }
    

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

