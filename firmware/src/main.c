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
uint8_t selectedRow = 0;
uint8_t previousRow = 0;
uint8_t upFlg = 0;
uint8_t downFlg = 0;
int rowhighlighted = 0;
int screen = 0;
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

typedef struct
{
    uint8_t previousState;
    uint8_t currentState;
    uint8_t Flag;
}ButtonStatus;

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

ButtonStatus buttonPress(ButtonStatus button, uint16_t pin){
    button.currentState = PORT_PinRead(pin);
        if((button.currentState == 1) && (button.previousState == 0)){
             HRTBEAT_LED_Toggle();
             button.Flag = 1;
             printf("set Flag");

        }
        button.previousState = button.currentState;
        return button;
}

int main ( void )
{
   
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    //Start Interrupt
    TC3_TimerCallbackRegister(TC3_Callback_InterruptHandler, (uintptr_t)NULL);
    
    TC3_TimerStart();
    
    //Timer Counters
    counter10ms = 0;
    counter100ms = 0;
    counter1S = 0;
    int timeofstate = 0;
    int buttonPressTime = 0;
    
    //State Machine States
    DISPLAY_STATES  stateMachine = START;
    DISPLAY_STATES  previousState = CLEANING;           //TODO Going back to beginning of state or middle?
    //System Flags
    SYSTEMS_CHECK_FLAGS sysCheckFlgs = {0, 0, 0, 0, 0, 0, 0};   // Init all flags to 0
    //Button statuses
    ButtonStatus powerButton = {0,0,0};
    ButtonStatus SelButton = {0,0,0};
    ButtonStatus NavAButton = {0,0,0};
    ButtonStatus NavBButton = {0,0,0};
    ButtonStatus EjectButton = {0,0,0};
    uint8_t asleepFlg = 0;
    
    HRTBEAT_LED_Set();
    
    I2C_Reset_Set();        // Keep Reset LED high

    //stateMachineLoop(stateMachine);    // TODO don't need to do this anymore Setup Display before writing to it
     
    while(true)
    {   
         /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        //Get Button Press Status
        powerButton = buttonPress(powerButton, POWER_BTN_PIN);
        SelButton = buttonPress(SelButton, SELECT_BTN_PIN);
        NavAButton = buttonPress(NavAButton, NAV_BTN_A_PIN);
        NavBButton = buttonPress(NavBButton, NAV_BTN_B_PIN);
        EjectButton = buttonPress(EjectButton, EJECT_BTN_PIN);
        
        //Asleep Mode
        if(SelButton.Flag == 1 || NavAButton.Flag == 1 || NavBButton.Flag == 1 || EjectButton.Flag == 1)     //Button pressed, recount time
        {
            if(asleepFlg == 1){     // screen has been asleep, turn on
                sendCommand(setDisplayON);      //turn on screen
                asleepFlg = 0;  // screen no longer asleep
                SelButton.Flag = 0; // clear all button presses so they don't affect current state
                NavAButton.Flag = 0; 
                NavBButton.Flag = 0;
                EjectButton.Flag = 0;
            }
            buttonPressTime = 0;    // Clear Count
        }
        else{
            if(buttonPressTime >= TimeOutValue){    //timeout, screen Sleeps
                sendCommand(setDisplayOFF);         //turn off screen = asleep
                asleepFlg = 1;                      //flag indicates screen is asleep
            }
        }
        
        //10 ms Condition
        if(counter10ms >= 1){
            //printf("10ms");
            counter10ms = 0;
            counter100ms++;        //count to 1S    
        }
        //100 ms Condition
        if(counter100ms >= 10){
            
            counter100ms = 0;
            counter1S++;        //count to 1S   
        }
        //1 s Condition
        if(counter1S >= 1){
            switch(stateMachine){
                case START:                     // No screen, wait for Power Button to transition
                    if(powerButton.Flag == 1){
                        stateMachine = SETUP;
                        timeofstate = 0;
                    }
                    break;
                case ABC:   // TODO Can remove this
                    if(timeofstate >= 5){
                        stateMachine = IDLE;
                    }
                    break;
                case SETUP:     // Inital setup for display
                    if(timeofstate >= 5){
                        stateMachine = IDLE;
                    }
                    break;
                case IDLE:
                    if(NavAButton.Flag == 1 || NavBButton.Flag == 1){   //Toggle Selections
                        if(selectedRow == 1){
                            selectedRow = 0;
                        }else if(selectedRow == 0){
                            selectedRow = 1;
                        }
                    }
                    
                    if(SelButton.Flag == 1){
                        if(selectedRow == 0){   // Row 1
                            stateMachine = SYSTEMSCHECKS; //De-scale Cycle selected, jump to screen
                            timeofstate = 0;
                            selectedRow = 0;
                        }else if(selectedRow == 1){ // Row 2
                            stateMachine = DEVICEMENU;    //Time Date selected, jump to screen
                            rowhighlighted = 0;
                            selectedRow = 0;
                            screen = 0;
                        }              //  Reset time of state
                    }
                    
                    if(EjectButton.Flag == 1){
                        stateMachine = DISCONNECT;
                        timeofstate = 0;
                    }
                    break;

                case DEVICEMENU:    // TODO implement Vertical Scrolling
                    if(NavAButton.Flag == 1){   //Scroll Up
                        upFlg = 1;
                        if(selectedRow > 0){
                            selectedRow--;      //Row selected decreases
                        }
                    }
                    
                    if(NavBButton.Flag == 1){   //Scroll 
                        downFlg = 1;
                        if(selectedRow < 6){   
                            selectedRow++;      //Row selected increases
                        }
                    }
                    
                    if(SelButton.Flag == 1){
                        if(selectedRow == 0){   //Row 1
                            stateMachine = AIRFILTER;   //Change Air Filter selected, jump to screen
                            previousRow = selectedRow;
                            selectedRow = 0;
                        }else if(selectedRow == 1){ // Row 2
                            stateMachine = WATERFILTER; //Change Water Filter selected, jump to screen
                            previousRow = selectedRow;
                            selectedRow = 0;
                        }else if(selectedRow == 2){ // Row 3
                            stateMachine = PURGE;   //Purge Cycle selected, jump to screen
                            previousRow = selectedRow;
                            selectedRow = 0;
                        }else if(selectedRow == 3){ // Row 3
                            stateMachine = DESCALE;   //Purge Cycle selected, jump to screen
                            previousRow = selectedRow;
                            selectedRow = 0;
                        }else if(selectedRow == 4){ // Row 3
                            stateMachine = TIMEDATE;   //Purge Cycle selected, jump to screen
                            previousRow = selectedRow;
                            selectedRow = 0;
                        }else if(selectedRow == 5){ // Row 5
                            stateMachine = IDLE;   
                            previousRow = selectedRow;
                            selectedRow = 0;
                        }
                        timeofstate = 0;        //TODO important? Needed? Should this be at the end of every switch                  
                    }
                    break;
                
                case AIRFILTER:
                    if(NavAButton.Flag == 1 || NavBButton.Flag == 1){   //Toggle Selections
                        if(selectedRow == 1){
                            selectedRow = 0;
                        }else if(selectedRow == 0){
                            selectedRow = 1;
                        }
                    }
                    
                    if(SelButton.Flag == 1){
                        if(selectedRow == 0){
                            stateMachine = IDLE;    // Finished, go back to IDLE screen
                            selectedRow = 0;
                        }else if(selectedRow == 1){
                            stateMachine = DEVICEMENU;  // Cancel, go back to device menu
                            selectedRow = previousRow;
                            //selectedRow = 0;        // Selected Row should be previous selection
                        }
                        timeofstate = 0;                      
                    }

                    break;
                case WATERFILTER:
                    if(NavAButton.Flag == 1 || NavBButton.Flag == 1){   //Toggle Selections
                        if(selectedRow == 1){
                            selectedRow = 0;
                        }else if(selectedRow == 0){
                            selectedRow = 1;
                        }
                    }
                    
                    if(SelButton.Flag == 1){
                        if(selectedRow == 0){
                            stateMachine = IDLE;    // Finished, go back to IDLE screen
                            selectedRow = 0;
                        }else if(selectedRow == 1){
                            stateMachine = DEVICEMENU; // Cancel, go back to device menu
                            selectedRow = previousRow;        // Selected Row should be previous selection
                        }
                        timeofstate = 0;                      
                    }
                    break;
                case PURGE:
                    if(NavAButton.Flag == 1 || NavBButton.Flag == 1){ //Toggle Selections
                        if(selectedRow == 1){
                            selectedRow = 0;
                        }else if(selectedRow == 0){
                            selectedRow = 1;
                        }
                    }
                    if(SelButton.Flag == 1){
                        if(selectedRow == 0){
                            stateMachine = PURGING;        //Run Purge Cycle
                            selectedRow = 0;
                        }else if(selectedRow == 1){
                            stateMachine = DEVICEMENU;  // Cancel, go back to device menu
                            selectedRow = previousRow;        // Selected Row should be previous selection
                        }
                        timeofstate = 0;                      
                    }
                    break;
                case DESCALE:
                    if(NavAButton.Flag == 1 || NavBButton.Flag == 1){   // Toggle Selections
                        if(selectedRow == 1){
                            selectedRow = 0;
                        }else if(selectedRow == 0){
                            selectedRow = 1;
                        }
                    }
                    if(SelButton.Flag == 1){
                        if(selectedRow == 0){
                            stateMachine = DESCALING;        //Run De-scale Cycle
                            selectedRow = 0;
                        }else if(selectedRow == 1){
                            stateMachine = DEVICEMENU; // Cancel, go back to device menu
                            selectedRow = previousRow;        // Selected Row should be previous selection
                        }
                        timeofstate = 0;                      
                    }
                    break;
                case PURGING:
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = COMPLETEDPURGE;
                        timeofstate = 0;
                    }
                    break;
                case DESCALING:
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = COMPLETEDDESCALE;
                        timeofstate = 0;
                    }
                    break;
                case COMPLETEDPURGE:
                    if(SelButton.Flag == 1){    // Purge Cycle Completed
                        stateMachine = IDLE;    // Jump to IDLE screen
                    }
                    break;
                case COMPLETEDDESCALE:
                    if(SelButton.Flag == 1){    // De-scale cycle Completed
                        stateMachine = IDLE;    // Jump to IDLE screen
                    }
                    break;
                case TIMEDATE:  // TODO, Functionality needs to be decided
                    if(SelButton.Flag == 1){ 
                        if(selectedRow == 0){
                            stateMachine = DEVICEMENU;
                           selectedRow = previousRow;
                        }
                        timeofstate = 0;                      
                    }
                    break;
                case SYSTEMSCHECKS:
                    if(timeofstate >= 3){
                        if(SelButton.Flag == 1){
                            stateMachine = CANCELSEL;  // Cancel System Checks
                        }
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
                            if(timeofstate >= SystemChecksTime){
                                stateMachine = CLEANING;    // All Checks Passed, move on to Cleaning Cycle State
                                timeofstate = 0;
                            }
                        }
                    }
                    
                    break;
                case PODMISSING:
                    if(SelButton.Flag == 1){        
                        stateMachine = IDLE;        // Back to Idle Screen
                    }
                    break;
                case PODEXHAUSTED:
                    if(SelButton.Flag == 1){
                        stateMachine = IDLE;        // Back to Idle Screen
                    }
                    break;
                case CASEMISSING:
                    if(SelButton.Flag == 1){
                        stateMachine = IDLE;        // Back to Idle Screen
                    }
                    break;
                case CATHMISSING:
                    if(SelButton.Flag == 1){
                        stateMachine = IDLE;        // Back to Idle Screen
                    }
                    break;
                case CATHEXHAUSTED:
                    if(SelButton.Flag == 1){
                        stateMachine = IDLE;        // Back to Idle Screen
                    }
                    break;
                case LEAKING:
                    if(SelButton.Flag == 1){
                        stateMachine = IDLE;        // Back to Idle Screen
                    }
                    break;
                case WASTEWATERRESERVOIR:
                    if(SelButton.Flag == 1){
                        stateMachine = IDLE;        // Back to Idle Screen
                    }
                    break;
                case CLEANING:
                    if(SelButton.Flag == 1){
                        stateMachine = CANCELSEL;  // Option to Cancel Cleaning Cycle
                        previousState = CLEANING;   // Save Previous State incase cancel not confirmed
                    }
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = DISINFECTION;
                        timeofstate = 0;
                    }
                    break;
                case DISINFECTION:
                    if(SelButton.Flag == 1){
                        stateMachine = CANCELSEL;  // Option to Cancel Cleaning Cycle
                        previousState = DISINFECTION; // Save Previous State incase cancel not confirmed
                    }
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = DRYING;
                        timeofstate = 0;
                    }
                    break;
                case DRYING:
                    if(SelButton.Flag == 1){
                        stateMachine = CANCELSEL; // Option to Cancel Cleaning Cycle
                        previousState = DRYING; // Save Previous State incase cancel not confirmed
                    }
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = COMPLETE;
                        timeofstate = 0;
                    }
                    break;
                case CANCELSEL:        // TODO combine CancelSel 0 and 1
                    if(NavAButton.Flag == 1 || NavBButton.Flag == 1){   // Toggle Selections
                        if(selectedRow == 1){
                            selectedRow = 0;
                        }else if(selectedRow == 0){
                            selectedRow = 1;
                        }
                    }
                    
                    if(SelButton.Flag == 1){
                        if(selectedRow == 0){
                            stateMachine = CANCELLED;
                            timeofstate = 0;
                        }
                        else{
                            stateMachine = previousState;   //Go back to state before cancel
                        }
                    }
                    
                    break;
                    
                case COMPLETE:
                    if(EjectButton.Flag == 1){
                        stateMachine = LUBRICATION; // Lubricate and Eject
                    }
                    break;
                case CANCELLED:
                    if(timeofstate >= 5){               //TODO random value to trigger change
                        stateMachine = CANCELPURGE;
                        timeofstate = 0;
                    }
                    break;
                case CANCELPURGE:
                    if(SelButton.Flag == 1){
                        stateMachine = IDLE;    // Cleaning Cycle Cancelled, return to IDLE
                    }
                    break;
                case LUBRICATION:
                    if(timeofstate >= LubricationTime){               //TODO random value to trigger change
                        stateMachine = DISCONNECT;      // After Lubrication, ready to disconnect
                        timeofstate = 0;
                    }
                    break;
                case DISCONNECT:
                    EJECT_Set();    // Light Indicates Eject
                    if(sysCheckFlgs.caseMissing == 0 && timeofstate >= SafeDisconnectTime){               //TODO random value to trigger , what triggers this?
                        stateMachine = IDLE;        // Jump to IDLE screen when done
                        //Eject Latch can close     
                        EJECT_Clear();              // Case hasn't been ejected
                        timeofstate = 0;
                    }
                    else if(sysCheckFlgs.caseMissing == 1){
                        stateMachine = IDLE;
                        //Eject Latch stays open    // Case was Ejected
                        timeofstate = 0;
                    }
                    break;
                default:
                    break;
                            
            }
            
            stateMachineLoop(stateMachine);  //Update screen based off state
            //Clear Flags between States
            SelButton.Flag = 0;                     //TODO Create flag struct
            NavAButton.Flag = 0;
            NavBButton.Flag = 0;
            EjectButton.Flag = 0;
            ////////////
            counter1S = 0;
            timeofstate++;
            buttonPressTime++;
        }

    } 
    

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

