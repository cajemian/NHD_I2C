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
    IDLE,
    STOP

} DISPLAY_STATES;



//10ms Interrupt
void TC3_Callback_InterruptHandler(TC_TIMER_STATUS status, uintptr_t context)
{   //TODO Change to 10ms and toggle Valves
    /* Toggle LED, Visible Tick */     
    counter10ms++; 

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
    
    while(true)
    {   
         /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
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
            printf("1s\r\n");
            counter1S = 0;
        }
        

            switch(stateMachine){
                case SETUP:
                    for(int i = 0; i < SETUP_COMMANDS; i++){
                        sendCommand(setupData[i]);
                    }
                    stateMachine = CLEAR;
                break;

                case CLEAR:
                    for(int i = 0; i < 4; i++){

                        setStartPage(i);
                        setStartColumn(4);

                        for(int i = 0; i < 132; i++){
                            sendData(0x00);
                        }
                    }
                    stateMachine = DRAW;
                break;

                case DRAW:
                    setPageAddress(0, 3);
                    setColumnAddress(4, 131);
                    setAddressingMode(0x00);

                    //write data
                      for(int i = 0; i < 516; i++){
                          sendData(logo[i]);
                      }
                    stateMachine = POWERUP;

                case POWERUP:
                    setPageAddress(0, 3);
                    setColumnAddress(4, 131);
                    setAddressingMode(0x00);

                    //write data
                      for(int i = 0; i < 516; i++){
                          sendData(PowerUp[i]);
                      }
                    stateMachine = IDLE;
                    break;
                case IDLE:
                    setPageAddress(0, 3);
                    setColumnAddress(4, 131);
                    setAddressingMode(0x00);

                    //write data
                      for(int i = 0; i < 516; i++){
                          sendData(IDLEscreen[i]);
                      }
                    stateMachine = STOP;
                    break;
                case STOP:
                    break;
            }
        }
    

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

