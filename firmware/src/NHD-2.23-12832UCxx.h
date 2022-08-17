/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

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
//define LED_ON()                       LED_Clear()
//#define LED_OFF()                      LED_Set()

#define APP_AT24MAC_DEVICE_ADDR             0x003D
#define APP_AT24MAC_MEMORY_ADDR             0x00
#define APP_AT24MAC_MEMORY_ADDR1            0x00
#define APP_TRANSMIT_DATA_LENGTH            2//6
#define APP_ACK_DATA_LENGTH                 1
#define APP_RECEIVE_DUMMY_WRITE_LENGTH      2
#define APP_RECEIVE_DATA_LENGTH             4
#define SETUP_COMMANDS                      33
#define CLEAR_COMMANDS                      12

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

#define setAddressingMode 0x20
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
    
static uint8_t setupData[SETUP_COMMANDS] = {
    setDisplayClock, RatioFrequency, setMultiplexRation, setMultiplexRation2, setDisplayOffset, setDisplayOffset2,
    setStartLine, setMasterConfig, setMasterConfig2, setAreaColor, setAreaColor2, setAddressingMode, setAddressingMode2, setSegmentRemap,
    setComRemap, setComConfig, setComConfig1, setLUT, setLUT1, setLUT1, setLUT1, setLUT1,
    setContrast, setContrast2, setBrightness, setBrightness2, setPrechargePeriod, setPrechargePeriod2, setVCOMH, setVCOMH2, setEntireDisplay, setInverseDisplay, setDisplayON
};



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


#define I2C_SLAVE_ADDR 0x3D
    

#define WHITE 1
#define Black 0


int I2C(uint8_t type, uint8_t data);          //returns 0 when in process
                                //returns 1 when done sending
//void I2C(APP_STATES state, uint8_t data);
    
    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************

    /*  A brief description of a section can be given directly below the section
        banner.
     */


    // *****************************************************************************

    /** Descriptive Data Type Name

      @Summary
        Brief one-line summary of the data type.
    
      @Description
        Full description, explaining the purpose and usage of the data type.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

      @Remarks
        Any additional remarks
        <p>
        Describe enumeration elements and structure and union members above each 
        element or member.
     */


    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

    /*  A brief description of a section can be given directly below the section
        banner.
     */

    // *****************************************************************************
    /**
      @Function
        int ExampleFunctionName ( int param1, int param2 ) 

      @Summary
        Brief one-line description of the function.

      @Description
        Full description, explaining the purpose and usage of the function.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

      @Precondition
        List and describe any required preconditions. If there are no preconditions,
        enter "None."

      @Parameters
        @param param1 Describe the first parameter to the function.
    
        @param param2 Describe the second parameter to the function.

      @Returns
        List (if feasible) and describe the return values of the function.
        <ul>
          <li>1   Indicates an error occurred
          <li>0   Indicates an error did not occur
        </ul>

      @Remarks
        Describe any special behavior not described above.
        <p>
        Any additional remarks.

      @Example
        @code
        if(ExampleFunctionName(1, 2) == 0)
        {
            return 3;
        }
     */
    


void Checkerboard_12832();
void Fill_RAM_12832(uint8_t Data);
void Set_Start_Page_12832(uint8_t d);
void Set_Start_Column_12832(uint8_t d);
void comm_out(uint8_t d);
void data_out(uint8_t d);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
