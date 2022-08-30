/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    NHD-2.23-12832UCxx.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _NHD_H    /* Guard against multiple inclusion */
#define _NHD_H

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */
#define WHITE 1
#define Black 0
    
#define SSD1305_HEIGHT                      32
#define SSD1305_WIDTH                       128
#define SSD1305_BUFFER_SIZE                 SSD1305_WIDTH * SSD1305_HEIGHT / 8
    
#define APP_AT24MAC_DEVICE_ADDR             0x003D
#define APP_AT24MAC_MEMORY_ADDR             0x00
#define APP_AT24MAC_MEMORY_ADDR1            0x00
#define APP_TRANSMIT_DATA_LENGTH            2
#define APP_ACK_DATA_LENGTH                 1
#define APP_RECEIVE_DUMMY_WRITE_LENGTH      2
#define APP_RECEIVE_DATA_LENGTH             4
#define SETUP_COMMANDS                      33

//Setup Commands Defined
#define setDisplayClock 0xD5
#define RatioFrequency 0x01

#define setMultiplexRation 0xA8
#define setMultiplexRation2 0x1F

#define setDisplayOffset 0xD3
#define setDisplayOffset2 0x00

#define setStartLine 0x40

#define setMasterConfig 0xAD
#define setMasterConfig2 0x8E

#define setAreaColor 0xD8
#define setAreaColor2 0x05

#define setAddressingMode1 0x20
#define setAddressingMode2 0x02
#define setSegmentRemap 0xA1

#define setComRemap 0xC8
#define setComConfig 0xDA
#define setComConfig1 0x10

#define setLUT 0x91
#define setLUT1 0x3F

#define setContrast 0x81
#define setContrast2 0xBF

#define setBrightness 0x82
#define setBrightness2 0xBF

#define setPrechargePeriod 0xD9
#define setPrechargePeriod2 0xD2

#define setVCOMH 0xD8
#define setVCOMH2 0x08

#define setEntireDisplay 0xA4
#define setInverseDisplay 0xA6

#define setDisplayON 0xAF
    
typedef enum
{
    SETUP,
    CLEAR,
    ABC,
    POWERUP,
    RUNCYCLE,
    MENU,
    SYSTEMSCHECKS,
            

} DISPLAY_STATES;

typedef enum
{
    APP_STATE_STATUS_VERIFY,
    APP_STATE_COMMAND_WRITE,                         //write command
    APP_STATE_DATA_WRITE,                           //write data 
    APP_STATE_EEPROM_WAIT_WRITE_COMPLETE,
    APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS,
    APP_STATE_EEPROM_READ,
    APP_STATE_EEPROM_WAIT_READ_COMPLETE,
    APP_SETUP,
    APP_STATE_IDLE,
    APP_STATE_XFER_SUCCESSFUL,
    APP_STATE_XFER_ERROR

} APP_STATES;

typedef enum
{
    APP_TRANSFER_STATUS_IN_PROGRESS,
    APP_TRANSFER_STATUS_SUCCESS,
    APP_TRANSFER_STATUS_ERROR,
    APP_TRANSFER_STATUS_IDLE,

} APP_TRANSFER_STATUS;

typedef enum
{
    APP_DATA_SETUP,
    APP_DATA_CLEAR,
    APP_DATA_DRAW,
         

} APP_DATA_SEND;

typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Initialized;
    uint8_t DisplayOn;
} SSD1306_t;

typedef struct {
	uint8_t x1;    
	uint8_t y1;   
    uint8_t x2;
    uint8_t y2;
} COORDINATES;


//Functions for navigating I2C and States
int I2C(uint8_t type, uint8_t data);          //returns 0 when in process
void stateMachineLoop(DISPLAY_STATES  stateMachine);

//Functions for Writing to the Screen
void writeData();
void ssd1305_DrawPixel(uint8_t x, uint8_t y, int color);
//void ssd1305_WriteChar(FontDef Font, char ch, int color);
void drawString(char *string, int color);
void ssd1305_Fill();
void ssd1305_SetCursor(uint8_t x, uint8_t y);
void ssd1305_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color);
void ssd1305_DrawRectangle(COORDINATES coordinates, int color);

//Functions for sending Commands and Startup
void sendCommand(uint8_t data); // returns 1 when done sending
void setStartPage(uint8_t pageAddr);    // Not Used
void setStartColumn(uint8_t address);   // Not Used
void setPageAddress(uint8_t startAddr, uint8_t endAddr);
void setColumnAddress(uint8_t startAddr, uint8_t endAddr);
void setAddressingMode(uint8_t mode);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
