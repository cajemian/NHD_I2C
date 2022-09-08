/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    NHD-2.23-12832UCxx.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "NHD-2.23-12832UCxx.h"
#include "string.h"
#include "fonts.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// Screenbuffer
static uint8_t SSD1305_Buffer[SSD1305_BUFFER_SIZE];

// Screen object
static SSD1306_t SSD1306;

static uint8_t setupData[SETUP_COMMANDS] = {
    setDisplayClock, RatioFrequency, setMultiplexRation, setMultiplexRation2, setDisplayOffset, setDisplayOffset2,
    setStartLine, setMasterConfig, setMasterConfig2, setAreaColor, setAreaColor2, setAddressingMode1, setAddressingMode2, setSegmentRemap,
    setComRemap, setComConfig, setComConfig1, setLUT, setLUT1, setLUT1, setLUT1, setLUT1,
    setContrast, setContrast2, setBrightness, setBrightness2, setPrechargePeriod, setPrechargePeriod2, setVCOMH, setVCOMH2, setEntireDisplay, setInverseDisplay, setDisplayON
};

static uint8_t commandTxData[APP_TRANSMIT_DATA_LENGTH] =
{
    0x00, 0x00
};

static uint8_t writeTxData[APP_TRANSMIT_DATA_LENGTH] =
{
    0x40, 0x00
};

static uint8_t  testRxData[APP_RECEIVE_DATA_LENGTH];

APP_STATES state = APP_STATE_STATUS_VERIFY;

volatile APP_TRANSFER_STATUS transferStatus = APP_TRANSFER_STATUS_ERROR;

uint8_t ackData = 0;
int count = 0;
int clearcount = 0;

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/***************************** APP_I2CCallback(context) **********************
 * Description: 
 * Inputs: uintptr_t
 * Outputs: None
 ****************************************************************/
void APP_I2CCallback(uintptr_t context )
{
    APP_TRANSFER_STATUS* transferStatus = (APP_TRANSFER_STATUS*)context;

    if(SERCOM2_I2C_ErrorGet() == SERCOM_I2C_ERROR_NONE)
    {
        if (transferStatus)
        {
            *transferStatus = APP_TRANSFER_STATUS_SUCCESS;
        }
    }
    else
    {
        if (transferStatus)
        {
            *transferStatus = APP_TRANSFER_STATUS_ERROR;
        }
    }
}

/***************************** I2C(type, data) **********************
 * Description: 
 * Inputs: uint8_t, uint8_t
 * Outputs: None
 ****************************************************************/
int I2C(uint8_t type, uint8_t data){
    switch (state)
        {
            case APP_STATE_STATUS_VERIFY:
                //Sending Ack Data from this state
                /* Register the TWIHS Callback with transfer status as context */
                SERCOM2_I2C_CallbackRegister( APP_I2CCallback, (uintptr_t)&transferStatus );

               /* Verify if EEPROM is ready to accept new requests */
                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                
                if(type == 'D'){
                    state = APP_STATE_DATA_WRITE;
                }
                else{
                    state = APP_STATE_COMMAND_WRITE;
                }

                break;

            //send Commands 0x00
            case APP_STATE_COMMAND_WRITE:
                commandTxData[1] = data; 
                //Always writing from this state
                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    /* Write data to EEPROM */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &commandTxData[0], APP_TRANSMIT_DATA_LENGTH);
                    state = APP_STATE_EEPROM_WAIT_WRITE_COMPLETE;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM is not ready to accept new requests. 
                     * Keep checking until the EEPROM becomes ready. */
                    state = APP_STATE_STATUS_VERIFY;
                }
                break;
            //Send Data 0x40
            case APP_STATE_DATA_WRITE:
                //Always writing from this state
                writeTxData[1] = data; 
                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    /* Write data to EEPROM */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &writeTxData[0], APP_TRANSMIT_DATA_LENGTH);
                    state = APP_STATE_EEPROM_WAIT_WRITE_COMPLETE;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM is not ready to accept new requests. 
                     * Keep checking until the EEPROM becomes ready. */
                    state = APP_STATE_STATUS_VERIFY;
                }
                break;

            case APP_STATE_EEPROM_WAIT_WRITE_COMPLETE:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    /* Read the status of internal write cycle */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    //SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                    state = APP_STATE_XFER_SUCCESSFUL;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    state = APP_STATE_XFER_ERROR;
                }
                break;

             case APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS:                 

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    
                    state = APP_STATE_EEPROM_WAIT_WRITE_COMPLETE;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    HRTBEAT_LED_Set();  //CAA Error
                    /* EEPROM's internal write cycle is not complete. Keep checking. */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                }
                break;

            case APP_STATE_EEPROM_READ:

                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                /* Read the data from the page written earlier */
                SERCOM2_I2C_WriteRead(APP_AT24MAC_DEVICE_ADDR, &commandTxData[0], APP_RECEIVE_DUMMY_WRITE_LENGTH,  &testRxData[0], APP_RECEIVE_DATA_LENGTH);

                state = APP_STATE_EEPROM_WAIT_READ_COMPLETE;

                break;

            case APP_STATE_EEPROM_WAIT_READ_COMPLETE:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    state = APP_SETUP;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_SETUP:
               
                  
                break;

            case APP_STATE_XFER_SUCCESSFUL:
            {
                //HRTBEAT_LED_Clear();
                state = APP_STATE_STATUS_VERIFY;
                return 1;
                break;
            }
            case APP_STATE_XFER_ERROR:
            {
                //HRTBEAT_LED_Set();
                return 1;
                break;
            }
            default:
                break;
        }
    return 0;
    }

/***************************** stateMachineLoop(screenState) **********************
 * Description: 
 * Inputs: DISPLAY_STATES
 * Outputs: None
 ****************************************************************/
void stateMachineLoop(DISPLAY_STATES  screenState){
    int color = 0;
    switch(screenState){
        case START:
            //No screens or setup, wait for powerbutton
            break;
        case SETUP:
            for(int i = 0; i < SETUP_COMMANDS; i++){
                sendCommand(setupData[i]);
            }
            
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);
            ssd1305_SetCursor(0,1);
            ssd1305_Fill();             // Clear Screen to start
            
            ssd1305_SetCursor(46,12);
            drawString(welcome, 0);
            
            writeData();

        break;
            
        case ABC:   // Can Delete
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);

            ssd1305_SetCursor(0,0);
            ssd1305_Fill();
            
            drawString(ABCs, 0);        // A-M
            ssd1305_SetCursor(66,0);
            
            ssd1305_SetCursor(0,9);
            drawString(nums, 0);

            writeData();
            break;
            
        case IDLE:
            rewriteScreen();
           
            ssd1305_SetCursor(0,0);
            drawString(battery,0);      //Battery Symbols

            ssd1305_SetCursor(32,1);
            drawString(pod,0);          //POD-x|x
            
            ssd1305_SetCursor(81,1);
            drawString(cath,0);         //CATH-xx|xx
            
            ssd1305_SetCursor(0,13);
            drawString(rep,0);          //REPROCESSED-xx|xx|xxxx xx:xx
            ssd1305_SetCursor(106,13);
            drawString(time,0);
            
            if(selectedRow == 0){   // Row 1
               ssd1305_DrawRectangle(runRect, 0);   //Highlight and Invert Row 1
               color = 1;   
            }
            else{
                color = 0;
            }
            ssd1305_SetCursor(15,23);
            drawString(runCycle,color);    // RUN CYCLE
            
            if(selectedRow == 1){   // Row 2
                ssd1305_DrawRectangle(menuRect, 0); //Highlight and Invert Row 2
                color = 1;
            }else{
                color = 0;
            }
            ssd1305_SetCursor(91, 23);
            drawString(menu,color);  // MENU  

            //Shapes & Lines
            ssd1305_Line(podLine.x1, podLine.y1, podLine.x2, podLine.y2, 0);   //Divider
            ssd1305_Line(cathLine.x1, cathLine.y1, cathLine.x2, cathLine.y2, 0);   //Divider
            ssd1305_Line(horizontalLine.x1, horizontalLine.y1, horizontalLine.x2, horizontalLine.y2, 0);   //Horizontal Line
            ssd1305_Line(monthLine.x1, monthLine.y1, monthLine.x2, monthLine.y2, 0);   //Divider
            ssd1305_Line(yearLine.x1, yearLine.y1, yearLine.x2, yearLine.y2, 0);   //Divider
            
            writeData();
            break;          
        case DEVICEMENU:
            
            rewriteScreen();
            color = 0;
            int rowColor[3] = {0, 0 ,0};
            switch(screen){
                case 0:
                    if(upFlg == 1 && rowhighlighted == 0){
                        //nothing
                    }
                    else if(upFlg == 1 && rowhighlighted <= 2){
                        rowhighlighted--;
                    }
                    else if(downFlg == 1 && rowhighlighted >= 2){
                        screen = 1;
                        rowhighlighted = 2;
                    }
                    else if(downFlg == 1 && rowhighlighted < 2){
                        rowhighlighted++;
                    }
                    rowColor[rowhighlighted] = 1;   //invert color of row highlighted
                    
                    ssd1305_SetCursor(menuRow1.x, menuRow1.y);
                    drawString(changeAir,rowColor[0]);    // CHANGE AIR FILTER
                    ssd1305_SetCursor(menuRow2.x, menuRow2.y);
                    drawString(changeWater,rowColor[1]);  // CHANGE WATER FILTER
                    ssd1305_SetCursor(menuRow3.x, menuRow3.y);
                    drawString(runPurge,rowColor[2]); // RUN PURGE CYCLE 
                    
                    ssd1305_DrawRectangle(rectScreen0[rowhighlighted], 0); //Highlight and Invert Row 2
                    
                    break;
                case 1:
                    if(upFlg == 1 && rowhighlighted == 0){
                        screen = 0;
                        rowhighlighted = 0;
                    }
                    else if(upFlg == 1 && rowhighlighted <= 2){
                        rowhighlighted--;
                    }
                    else if(downFlg == 1 && rowhighlighted >= 2){
                        screen = 2;
                        rowhighlighted = 2;
                    }
                    else if(downFlg == 1 && rowhighlighted < 2){
                        rowhighlighted++;
                    }
                    rowColor[rowhighlighted] = 1;   //invert color of row highlighted
                    
                    ssd1305_SetCursor(menuRow1.x, menuRow1.y);
                    drawString(changeWater,rowColor[0]);    // CHANGE WATER FILTER
                    ssd1305_SetCursor(menuRow2.x, menuRow2.y);
                    drawString(runPurge,rowColor[1]);  // RUN PURGE CYCLE 
                    ssd1305_SetCursor(menuRow3.x, menuRow3.y);
                    drawString(runDescale,rowColor[2]); // RUN DE-SCALE SELECT 
                    
                    ssd1305_DrawRectangle(rectScreen1[rowhighlighted], 0); //Highlight and Invert Row 2
                    
                    break;
                case 2:
                    if(upFlg == 1 && rowhighlighted == 0){
                        screen = 1;
                        rowhighlighted = 0;
                    }
                    else if(upFlg == 1 && rowhighlighted <= 2){
                        rowhighlighted--;
                    }
                    else if(downFlg == 1 && rowhighlighted >= 2){
                        screen = 3;
                        rowhighlighted = 2;
                    }
                    else if(downFlg == 1 && rowhighlighted < 2){
                        rowhighlighted++;
                    }
                    rowColor[rowhighlighted] = 1;   //invert color of row highlighted
                    
                    ssd1305_SetCursor(menuRow1.x, menuRow1.y);
                    drawString(runPurge,rowColor[0]);    // RUN PURGE CYCLE 
                    ssd1305_SetCursor(menuRow2.x, menuRow2.y);
                    drawString(runDescale,rowColor[1]);  // RUN DE-SCALE SELECT
                    ssd1305_SetCursor(menuRow3.x, menuRow3.y);
                    drawString(setDateTime,rowColor[2]);  // SET TIME-DATE
                    
                    ssd1305_DrawRectangle(rectScreen2[rowhighlighted], 0); //Highlight and Invert Row 2
                    
                    break;
                case 3:
                    if(upFlg == 1 && rowhighlighted == 0){
                        screen = 2;
                        rowhighlighted = 0;
                    }
                    else if(upFlg == 1 && rowhighlighted <= 2){
                        rowhighlighted--;
                    }
                    else if(downFlg == 1 && rowhighlighted >= 2){
                        
                    }
                    else if(downFlg == 1 && rowhighlighted < 2){
                        rowhighlighted++;
                    }
                    rowColor[rowhighlighted] = 1;   //invert color of row highlighted
                    
                    ssd1305_SetCursor(menuRow1.x, menuRow1.y);
                    drawString(runDescale,rowColor[0]);    // RUN DE-SCALE SELECT
                    ssd1305_SetCursor(menuRow2.x, menuRow2.y);
                    drawString(setDateTime,rowColor[1]);  // SET TIME-DATE
                    ssd1305_SetCursor(menuRow3.x, menuRow3.y);
                    drawString(back,rowColor[2]);  // back
                    
                    ssd1305_DrawRectangle(rectScreen3[rowhighlighted], 0); //Highlight and Invert Row 2
                    break;
            }
            
            writeData();
            upFlg = 0;
            downFlg = 0;
            break;
            
        case AIRFILTER:
            rewriteScreen();
            
            ssd1305_SetCursor(row1.x,row1.y);
            drawString(changeAir,0);    // CHANGE AIR FILTER"
            
            ssd1305_SetCursor(row2.x,row2.y);
            drawString(removePanel,0);  // REMOVE PANEL 1 AND REPLACE
            
            if(selectedRow == 0){
               ssd1305_DrawRectangle(finishedRect, 0);  // Highlight and Invert
               color = 1;   
            }
            else{
                color = 0;
            }
            ssd1305_SetCursor(13,22);
            drawString(finished,color);  // FINISHED
            
            if(selectedRow == 1){
                ssd1305_DrawRectangle(cancelRect, 0); // Highlight and Invert
                color = 1;
            }else{
                color = 0;
            }
            ssd1305_SetCursor(87,22);
            drawString(cancel,color);   // CANCEL
            
            writeData();
            break;
            
        case WATERFILTER:
            rewriteScreen();
            
            ssd1305_SetCursor(row1.x, row1.y);
            drawString(changeWater,0); // CHANGE WATER FILTER
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(removePanel,0); // REMOVE PANEL 1 AND REPLACE
            
            if(selectedRow == 0){
               ssd1305_DrawRectangle(finishedRect, 0); // Highlight and Invert
               color = 1;   
            }
            else{
                color = 0;
            }
            ssd1305_SetCursor(13,22);
            drawString(finished,color);     // Finished
            
            if(selectedRow == 1){
                ssd1305_DrawRectangle(cancelRect, 0); // Highlight and Invert
                color = 1;
            }else{
                color = 0;
            }
            ssd1305_SetCursor(87,22);
            drawString(cancel,color);   // Cancel
            
            writeData();
            break;
            
        case PURGE:
            rewriteScreen();
            
            ssd1305_SetCursor(row1.x, row1.y);
            drawString(runPurge,0); // RUN PURGE CYCLE
            
            if(selectedRow == 0){
               ssd1305_DrawRectangle(startCycleRect, 0); // Highlight and Invert
               color = 1;   
            }
            else{
                color = 0;
            }
            ssd1305_SetCursor(13,22);
            drawString(startCycle,color);  // START CYCLE
            
            if(selectedRow == 1){
                ssd1305_DrawRectangle(cancelRect, 0); // Highlight and Invert
                color = 1;
            }else{
                color = 0;
            }
            ssd1305_SetCursor(87,22);
            drawString(cancel,color); // CANCEL
            
            writeData();
            break;
            
        case DESCALE:
            rewriteScreen();
            
            ssd1305_SetCursor(row1.x, row1.y);
            drawString(loadDescale,0); // LOAD DE-SCALE FLUID
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(runDescale,0); // RUN DE-SCALE CYCLE
            
            if(selectedRow == 0){
               ssd1305_DrawRectangle(startCycleRect, 0); // Highlight and Invert
               color = 1;   
            }
            else{
                color = 0;
            }
            ssd1305_SetCursor(13,22);
            drawString(startCycle,color); // START CYCLE
            
            if(selectedRow == 1){
                ssd1305_DrawRectangle(cancelRect, 0); // Highlight and Invert
                color = 1;
            }else{
                color = 0;
            }
            ssd1305_SetCursor(87,22);
            drawString(cancel,color); // Cancel
            
            writeData();
            break;
            
        case PURGING:
            rewriteScreen();
            
            ssd1305_SetCursor(row1.x, row1.y);
            drawString(purging, 0);    // STATUS:PURGING  

            writeData();
            break;
            
        case DESCALING:
            rewriteScreen();
            
            ssd1305_SetCursor(row1.x, row1.y);
            drawString(descaling, 0);    // STATUS:DESCALING

            writeData();
            break;
            
        case COMPLETEDPURGE:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(completedPurge, 0); // PURGE CYCLE COMPLETE
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE
            
            writeData();
            break;
        case COMPLETEDDESCALE:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(descalePurge, 0); // DESCALE CYCLE COMPLETE     
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0); // PRESS SELECT TO CONTINUE
            
            writeData();
            break;
            
        case TIMEDATE:
            rewriteScreen();
            
            if(selectedRow == 0){
                ssd1305_DrawRectangle(deviceMenu6, 0); // Highlight and Invert
                color = 1;
            }else{
                color = 0;
            }
            ssd1305_SetCursor(2,22);
            drawString(back,color);     //BACK
            
            writeData();
            break;
            
        case SYSTEMSCHECKS:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(systemChecks, 0);    // STATUS:SYSTEMS CHECK
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(systemChecks2, 0);   // CLEANING ABOUT TO START
            
            ssd1305_SetCursor(1,22);
            drawString(systemChecks3, 0);   // HOLD SELECT TO CANCEL CYCLE

            writeData();
            break;
            
        case PODMISSING:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(systemFailed, 0);    // STATUS:SYSTEM CHECK FAILED
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(podMissing, 0);   // POD MISSING:INSERT POD
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE

            writeData();
            break;
            
        case PODEXHAUSTED:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(systemFailed, 0);    // STATUS:SYSTEM CHECK FAILED
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(podExhausted, 0);   // POD EXHUASTED:INSERT NEW POD
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE

            writeData();
            break;
            
        case CASEMISSING:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(systemFailed, 0);    // STATUS:SYSTEM CHECK FAILED
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(caseMissing, 0);   // POD EXHUASTED:INSERT NEW POD
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE

            writeData();
            break;
            
        case CATHMISSING:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(systemFailed, 0);    // STATUS:SYSTEM CHECK FAILED
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(cathMissing, 0);   // CATHETER MISSING
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE

            writeData();
            break;
            
        case CATHEXHAUSTED:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(systemFailed, 0);    // STATUS:SYSTEM CHECK FAILED
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(cathExhausted, 0);   // CATHETER EXHAUSTED
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE

            writeData();
            break;
            
        case LEAKING:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(systemFailed, 0);    // STATUS:SYSTEM CHECK FAILED
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(leaking, 0);        // LEAK CHECK FAILED
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE

            writeData();
            break;
            
        case WASTEWATERRESERVOIR:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(systemFailed, 0);    // STATUS:SYSTEM CHECK FAILED
            
            ssd1305_SetCursor(row2.x, row2.y);
            drawString(reservoirCheck, 0);   // RESERVOIR CHECK FAILED
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE

            writeData();
            break;
            
        case CLEANING:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(cleaning, 0);    // STATUS:CLEANING

            ssd1305_SetCursor(1,22);
            drawString(holdCancel, 0);   // HOLD SELECT TO CANCEL CYCLE

            writeData();
            break;
            
        case DISINFECTION:
            rewriteScreen();
            
            ssd1305_SetCursor(row1.x, row1.y);
            drawString(disinfect, 0);    // STATUS:DISINFECTION
            
            ssd1305_SetCursor(1,22);
            drawString(holdCancel, 0);   // HOLD SELECT TO CANCEL CYCLE

            writeData();
            break;
            
        case DRYING:
            rewriteScreen();
            
            ssd1305_SetCursor(row1.x, row1.y);
            drawString(drying, 0);    // STATUS:DRYING
            
            ssd1305_SetCursor(1,22);
            drawString(holdCancel, 0);   // HOLD SELECT TO CANCEL CYCLE

            writeData();
            break;
            
        case CANCELSEL:    //TODO Combine
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(cancelCycle, 0);    // CANCEL THE CURRENT CYCLE?

            if(selectedRow == 0){
               ssd1305_DrawRectangle(yesRect, 0); // Highlight and Invert
               color = 1;   
            }
            else{
                color = 0;
            }
            ssd1305_SetCursor(30,22);
            drawString(yes, color);    // CONTINUE CYCLE
            
            if(selectedRow == 1){
               ssd1305_DrawRectangle(noRect, 0); // Highlight and Invert
               color = 1;   
            }
            else{
                color = 0;
            }
            ssd1305_SetCursor(89,22);
            drawString(no, color);   // CANCEL CYCLE
            
            writeData();
            break;
                
        case COMPLETE:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(complete, 0);    // STATUS:CLEANING COMPLETE

            ssd1305_SetCursor(row2.x, row2.y);
            drawString(dateTime, 0);    // DATE & TIME
            
            ssd1305_SetCursor(1,22);
            drawString(pressEject, 0);   // PRESS EJECT TO USE CATHETER
            
            //Draw Line
            ssd1305_Line(dateLine1.x1, dateLine1.y1, dateLine1.x2, dateLine1.y2, 0);
            ssd1305_Line(dateLine2.x1, dateLine2.y1, dateLine2.x2, dateLine2.y2, 0);

            writeData();
            break;
            
        case CANCELLED:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(cancelled, 0);    // CYCLE CANCELLED

            ssd1305_SetCursor(row2.x, row2.y);
            drawString(purge, 0);    // COMPLETING PURGE CYCLE
            
            ssd1305_SetCursor(1,22);
            drawString(pleaseWait, 0);   // PLEASE WAIT

            writeData();
            break;
            
        case CANCELPURGE:
            rewriteScreen();

            ssd1305_SetCursor(row1.x, row1.y);
            drawString(cancelled, 0);    // CYCLE CANCELLED

            ssd1305_SetCursor(row2.x, row2.y);
            drawString(empty, 0);    // EMPTY AND REFILL WATER
            
            ssd1305_SetCursor(1,22);
            drawString(pressSelect, 0);   // PRESS SELECT TO CONTINUE

            writeData();
            break;
            
        case LUBRICATION:
            rewriteScreen();
            
            ssd1305_SetCursor(12,12);     // Center
            drawString(processing, 0);    // STATUS: PROCESSING

            writeData();
            break;
            
        case DISCONNECT:
            rewriteScreen();
            
            ssd1305_SetCursor(11,12);
            drawString(disconnect, 0);    // SAFE TO REMOVE CASE NOW

            writeData();
            break;
        default:
            screenState = SETUP;
            break;
    }
        
}

/***************************** rewriteScreen() **********************
 * Description: Called before each screen written, need to reset x,y
 * Inputs:
 * Outputs: None
 ****************************************************************/
void rewriteScreen(){
    setPageAddress(0, 3);
    setColumnAddress(4, 131);
    //setAddressingMode(0x00);
    ssd1305_Fill();
}
/***************************** writeData() **********************
 * Description: 
 * Inputs:
 * Outputs: None
 ****************************************************************/
void writeData(){
    for(int i = 0; i<512; i++){
        int cmdFlg = 0;
        while(cmdFlg == 0){
            cmdFlg = I2C('D', SSD1305_Buffer[i]);
        }
    }
}

/***************************** ssd1305_DrawPixel() **********************
 * Description: 
 * Inputs: uint8_t, uint8_t, int
 * Outputs: None
 ****************************************************************/
void ssd1305_DrawPixel(uint8_t x, uint8_t y, int color) {
    if(x >= SSD1305_WIDTH || y >= SSD1305_HEIGHT) {
        // Don't write outside the buffer
        return;
    }
   
    // Draw in the right color
    if(color == 0) {
        SSD1305_Buffer[x + (y / 8) * SSD1305_WIDTH] |= 1 << (y % 8);
    } else { 
        SSD1305_Buffer[x + (y / 8) * SSD1305_WIDTH] &= ~(1 << (y % 8));
    }
}

/***************************** ssd1305_WriteChar() **********************
 * Description: 
 * Draw 1 char to the screen buffer
 * ch       => char om weg te schrijven
 * Font     => Font waarmee we gaan schrijven
 * color    => Black or White
 * Inputs: char, int
 * Outputs: None
 ****************************************************************/
void ssd1305_WriteChar(FontDef Font, char ch, int color) {
    //FontDef Font = Font_5x7;                //TODO Change font for M & W
    int b;
    //FontDef Font, SSD1306_COLOR color
    // Check if character is valid
    if (ch < 32 || ch > 126){
        return;
    }

    // Check remaining space on current line
    if (SSD1305_WIDTH < (SSD1306.CurrentX + Font.FontWidth) ||
        SSD1305_HEIGHT < (SSD1306.CurrentY + Font.FontHeight))
    {
        // Not enough space on current line
        return;
    }
    
    // Use the font to write
    for(int i = 0; i < Font.FontHeight; i++) {                      //7
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(int j = 0; j < Font.FontWidth; j++) {                   //10
            if((b << j) & 0x8000)  {
                ssd1305_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), color);
            } else {
                ssd1305_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), !color);
            }
        }
    }
    
    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;
}
    
/***************************** drawString() **********************
 * Description: 
 * Inputs: None
 * Outputs: None
 ****************************************************************/
void drawString(char *string, int color){
    int size = strlen(string);
    
    for(int i = 0; i < size; i++){
        if(string[i] == 'M' || string[i] == 'W' || string[i] ==  '-'){
            ssd1305_WriteChar(Font_6x7, string[i], color);
        }else if(string[i] == 'I' || string[i] == 'T' || string[i] == 'c' || string[i] == ':'){
            ssd1305_WriteChar(Font_4x7, string[i], color);
        }else if(string[i] == '|' || string[i] == 32 || string[i] == ';'){
            ssd1305_WriteChar(Font_2x7, string[i], color);
        }else{
            ssd1305_WriteChar(Font_5x7, string[i], color);
        }
    }
}

/***************************** ssd1305_Fill() **********************
 * Description: Fill the whole screen with the given color
 * Inputs: None
 * Outputs: None
 ****************************************************************/
void ssd1305_Fill() {
    /* Set memory */
    uint32_t i;
    for(i = 0; i < sizeof(SSD1305_Buffer); i++) {
        SSD1305_Buffer[i] = 0x00;
    }
}

/***************************** ssd1305_SetCursor() **********************
 * Description: Position the cursor
 * Inputs: None
 * Outputs: None
 ****************************************************************/
void ssd1305_SetCursor(uint8_t x, uint8_t y) {
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

/***************************** ssd1305_Line() **********************
 * Description: Draw line by Bresenhem's algorithm
 * Inputs: uint8_t, uint8_t, uint8_t, uint8_t, int
 * Outputs: None
 ****************************************************************/
void ssd1305_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color) {
  int32_t deltaX = abs(x2 - x1);
  int32_t deltaY = abs(y2 - y1);
  int32_t signX = ((x1 < x2) ? 1 : -1);
  int32_t signY = ((y1 < y2) ? 1 : -1);
  int32_t error = deltaX - deltaY;
  int32_t error2;
  
  ssd1305_DrawPixel(x2, y2, color);
    while((x1 != x2) || (y1 != y2))
    {
    ssd1305_DrawPixel(x1, y1, color);
    error2 = error * 2;
    if(error2 > -deltaY)
    {
      error -= deltaY;
      x1 += signX;
    }
    else
    {
    /*nothing to do*/
    }
        
    if(error2 < deltaX)
    {
      error += deltaX;
      y1 += signY;
    }
    else
    {
    /*nothing to do*/
    }
  }
  return;
}

/***************************** ssd1305_DrawRectangle() **********************
 * Description: Draw line by Bresenhem's algorithm
 * Inputs: COORDINATES, int
 * Outputs: None
 ****************************************************************/
void ssd1305_DrawRectangle(COORDINATES coordinates, int color) {
  ssd1305_Line(coordinates.x1,coordinates.y1,coordinates.x2,coordinates.y1, color);
  ssd1305_Line(coordinates.x2,coordinates.y1,coordinates.x2,coordinates.y2, color);
  ssd1305_Line(coordinates.x2,coordinates.y2,coordinates.x1,coordinates.y2, color);
  ssd1305_Line(coordinates.x1,coordinates.y2,coordinates.x1,coordinates.y1, color);

  return;
}

/***************************** sendCommand() **********************
 * Description: 
 * Inputs: uint8_t
 * Outputs: None
 ****************************************************************/
void sendCommand(uint8_t data){
    int cmdFlg = 0;
    while(cmdFlg == 0){
        cmdFlg = I2C('C', data);
    }
}

/***************************** sendCommand() **********************
 * Description: 
 * Inputs: uint8_t
 * Outputs: None
 ****************************************************************/
void sendData(uint8_t data){
    int cmdFlg = 0;
    while(cmdFlg == 0){
        cmdFlg = I2C('D', data);
    }
}

/***************************** setStartPage() **********************
 * Description: 
 * Inputs: uint8_t
 * Outputs: None
 ****************************************************************/
void setStartPage(uint8_t pageAddr){        //TODO Not Used
    sendCommand(0xB0 | pageAddr);
}

/***************************** setStartColumn() **********************
 * Description: 
 * Inputs: uint8_t
 * Outputs: None
 ****************************************************************/
void setStartColumn(uint8_t address){   // TODO Not Used
    sendCommand(0x00 + address % 16);
    sendCommand(0x10 | address / 16);
}

/***************************** setPageAddress() **********************
 * Description: 
 * Inputs: uint8_t, uint8_t
 * Outputs: None
 ****************************************************************/
void setPageAddress(uint8_t startAddr, uint8_t endAddr)
{
  sendCommand(0x22);
  sendCommand(startAddr);
  sendCommand(endAddr);
}

/***************************** setColumnAddress() **********************
 * Description: 
 * Inputs: uint8_t, uint8_t
 * Outputs: None
 ****************************************************************/
void setColumnAddress(uint8_t startAddr, uint8_t endAddr)
{
  sendCommand(0x21);
  sendCommand(startAddr);
  sendCommand(endAddr);
}

/***************************** setAddressingMode() **********************
 * Description: 
 * Inputs: uint8_t
 * Outputs: None
 ****************************************************************/
void setAddressingMode(uint8_t mode)
{
  sendCommand(0x20);
  sendCommand(mode);
}

/***************************** setAddressingMode() **********************
 * Description: 
 * Inputs: uint8_t
 * Outputs: None
 ****************************************************************/
void scrollEntireScreen()
{
  setStartPage(0);

  sendCommand(0x29); // Right horizontal scroll
  sendCommand(0x00);
  sendCommand(0x00);
  //sendCommand(0x01); // Scroll by 1 column
  //sendCommand(0x00); // Define PAGE0 as start page address
  sendCommand(0x11); // Set time interval between each scroll step as 6 frames
  sendCommand(0x03); // Define PAGE3 as end page address
  sendCommand(0x0a);
  sendCommand(0x2F); // Activate scrolling
}

/* *****************************************************************************
 End of File
 */
