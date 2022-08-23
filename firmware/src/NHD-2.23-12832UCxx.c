/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

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
#include "fonts.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */


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

//void I2C(APP_STATES state, uint8_t data){
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
                HRTBEAT_LED_Clear();
                state = APP_STATE_STATUS_VERIFY;
                return 1;
                break;
            }
            case APP_STATE_XFER_ERROR:
            {
                HRTBEAT_LED_Set();
                return 1;
                break;
            }
            default:
                break;
        }
    return 0;
    }

//draw just the letter A
void drawChar(){
    char ch = 'A';
    //FontDef Font = Font_7x10;
    colAddress = 4;
    pageAddress = 0;
   // FontDef Font = Font_5x8;
    
    //Check if character is valid
    if(ch < 32 || ch > 126){
        //return 0;
    }
    //TODO Check Remaining space on current line
    setPageAddress(0,3);
    for(int i = 0; i< 5; i++){//Font.FontWidth; i++){
        setColumnAddress(colAddress,131);
        colAddress++;
        
        sendData(Font5x8[i] & 0x00FF);                   //keep lower 8 bits
        
    }
    setPageAddress(1,3);
    colAddress = 4;
    for(int i = 0; i< 5; i++){//Font.FontWidth; i++){   //keep higher 8 bits
        setColumnAddress(colAddress,131);
        colAddress++;
        
        sendData(Font5x8[i] >> 8);
        
    }
    /*for(int i = 0; i < 4; i++){
        setPageAddress(i, 3);
        setColumnAddress(4, 10);
        setAddressingMode(0x00);
        sendData(0x7C);
        setColumnAddress(5, 10);
        sendData(0x12);
        setColumnAddress(6, 10);
        sendData(0x11);
        setColumnAddress(7, 10);
        sendData(0x12);
        setColumnAddress(8, 10);
        sendData(0x7C);
    }*/
}

void sendCommand(uint8_t data){
    int cmdFlg = 0;
    while(cmdFlg == 0){
        cmdFlg = I2C('C', data);
    }
}

void sendData(uint8_t data){
    int cmdFlg = 0;
    while(cmdFlg == 0){
        cmdFlg = I2C('D', data);
    }
}

void setStartPage(uint8_t pageAddr){
    sendCommand(0xB0 | pageAddr);
}

void setStartColumn(uint8_t address){
    sendCommand(0x00 + address % 16);
    sendCommand(0x10 | address / 16);
}

void setPageAddress(uint8_t startAddr, uint8_t endAddr)
{
  sendCommand(0x22);
  sendCommand(startAddr);
  sendCommand(endAddr);
}

void setColumnAddress(uint8_t startAddr, uint8_t endAddr)
{
  sendCommand(0x21);
  sendCommand(startAddr);
  sendCommand(endAddr);
}

void setAddressingMode(uint8_t mode)
{
  sendCommand(0x20);
  sendCommand(mode);
}

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

// *****************************************************************************

/** 
  @Function
    int ExampleInterfaceFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Remarks
    Refer to the example_file.h interface header for function usage details.
 */


/* *****************************************************************************
 End of File
 */
