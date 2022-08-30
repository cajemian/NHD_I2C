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
    switch(screenState){
        case SETUP:
            for(int i = 0; i < SETUP_COMMANDS; i++){
                sendCommand(setupData[i]);
            }
            
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);
            ssd1305_SetCursor(0,1);
            ssd1305_Fill();
            writeData();

        break;

        case CLEAR: //  TODO Cannot figure out how to clear screen after written to it
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);
            ssd1305_SetCursor(0,1);
            ssd1305_Fill();
            writeData();

        break;
            
        case POWERUP:
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);
            ssd1305_Fill();

            ssd1305_SetCursor(0,10);
            //char ABCs2[] = "MNOPQRSTUVW";
            drawString(pwrUp, 0);

            writeData();
            //screenState = LETTER;
            break;
            
        case ABC:
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);

            ssd1305_SetCursor(0,0);
            ssd1305_Fill();
            
            drawString(ABCs, 0);        // A-M
            ssd1305_SetCursor(66,0);  
            drawString(ABCs2, 0);        //N-W
            ssd1305_SetCursor(117,0);
            drawString(ABCs3, 0);        //XY
            
            ssd1305_SetCursor(0,9);
            drawString(nums, 0);

            writeData();
            break;
        case RUNCYCLE:
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);
            ssd1305_Fill();
            
            ssd1305_SetCursor(0,0);
            drawString(battery,0);
            
            ssd1305_SetCursor(78,0);
            drawString(cath,0);
            
            ssd1305_SetCursor(35,0);
            drawString(pod,0);
            
            ssd1305_Line(0,9,127,9, 0);
            
            ssd1305_SetCursor(0,11);
            drawString(rep,0);
            ssd1305_SetCursor(106,11);
            drawString(time,0);
            
            ssd1305_SetCursor(20,21);
            
            drawString(runCycle, 1);
            ssd1305_DrawRectangle(runRect, 0);
            ssd1305_SetCursor(74, 21);
            drawString(menu1, 0);
            ssd1305_SetCursor(80,21);
            drawString(menu2, 0);
            
            writeData();
            break;
        case MENU:
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);
            ssd1305_Fill();
            
            ssd1305_SetCursor(0,0);
            drawString(battery,0);
            
            ssd1305_SetCursor(78,0);

            drawString(cath,0);
            ssd1305_SetCursor(35,0);

            drawString(pod,0);
            
            ssd1305_Line(0,9,127,9, 0);
            
            ssd1305_SetCursor(0,11);
            drawString(rep,0);
            ssd1305_SetCursor(106,11);
            drawString(time,0);
            
            ssd1305_SetCursor(20,21);

            ssd1305_DrawRectangle(menuRect, 0);
            drawString(runCycle, 0);
            ssd1305_SetCursor(74,21);
            drawString(menu1, 1);
            ssd1305_SetCursor(80,21);
            drawString(menu2, 1);
            ssd1305_Line(79,21,79,27, 0);
 

            writeData();
            break;
        case SYSTEMSCHECKS:
            setPageAddress(0, 3);
            setColumnAddress(4, 131);
            setAddressingMode(0x00);

            ssd1305_SetCursor(15,0);
            ssd1305_Fill();
            drawString(systemChecks, 0);

            writeData();
            break;
        default:
            screenState = CLEAR;
            break;
    }
        
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
void ssd1305_WriteChar(char ch, int color) {
    FontDef Font = Font_5x7;                //TODO Change font for M & W
    int b;
    //FontDef Font, SSD1306_COLOR color
    // Check if character is valid
    if (ch < 32 || ch > 126){
       
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
        ssd1305_WriteChar(string[i], color);
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

/* *****************************************************************************
 End of File
 */
