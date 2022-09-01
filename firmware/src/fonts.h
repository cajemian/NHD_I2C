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

#ifndef _FONTS_H    /* Guard against multiple inclusion */
#define _FONTS_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "definitions.h"                // SYS function prototypes

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */
//#define SSD1305_INCLUDE_FONT_6x8
#define SSD1305_INCLUDE_FONT_7x10
    

    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************
char off []  = "OFF";
char pwrUp[] = "POWER UP";
char ABCs[] = "ABCDEFGHIJKLMNOPQRSTUVWXY";
char nums[] = "Z0123456789";
char runCycle[] = "RUN CYCLE";
char menu[] = "MENU";
//System Checks
char systemChecks[] = "STATUS:SYSTEMS CHECK";
char systemChecks2[] = "CLEANING ABOUT TO START";
char systemChecks3[] = "HOLD SELECT TO CANCEL CYCLE";   //TODO Repeat
char systemFailed[] = "STATUS:SYSTEM CHECK FAILED";
char pressSelect[] = "PRESS SELECT TO CONTINUE";
char podMissing[] = "POD MISSING:INSERT POD";
char podExhausted[] = "POD EXHUASTED:INSERT NEW POD";
char caseMissing[] =  "CASE MISSING:INSERT CASE";
char cathMissing[] = "CATHETER MISSING";    // Not done
char cathExhausted[] = "CATHETER EXHAUSTED";// Not done
char leaking[] = "LEAK CHECK FAILED";   // Not done
char reservoirCheck[] = "RESERVOIR CHECK FAILED";   // Not done
//Cleaning Cycle
char holdCancel[] = "HOLD SELECT TO CANCEL CYCLE";  //TODO Repeat
char cleaning[] =  "STATUS:CLEANING";
char disinfect[] = "STATUS:DISINFECTION";
char drying[] =    "STATUS:DRYING             ";    // Ending spaces to clear row
char complete[] = "STATUS:CLEANING COMPLETE";
char dateTime[] = "09|01|2022-09:30";
char pressEject[] = "PRESS EJECT TO USE CATHETER";
char cancelCycle[] = "CANCEL THE CURRENT CYCLE?";
char continueCycle[] = "CONTINUE CYCLE";
char cancelCycle2[] = "CANCEL CYCLE";
//Cancelled-Purge Cycle
char cancelled[] = "CYCLE CANCELLED";
char purge[] = "COMPLETING PURGE CYCLE";
char pleaseWait[] = "PLEASE WAIT";
char empty[] = "EMPTY AND REFILL WATER";


char battery[] = "abc";
char cath[] = "CATH-70|99";
char pod[] = "POD-4|5";
char rep[] = "REPROCESSED-08|29|2022";
char time[] = "09:30";

//Coordinates for Lines
COORDINATES runRect = {14, 22, 56, 30};
COORDINATES menuRect = {90, 22, 111, 30};
COORDINATES podLine = {58, 0, 58, 0};
COORDINATES cathLine = {116,0,116,8};
COORDINATES monthLine = {71,12,71,20};
COORDINATES yearLine = {83,12,83,20};
COORDINATES horizontalLine = {0,10,127,10,};
COORDINATES continueRect = {0, 21, 65, 29};                  //contiune Cycle
COORDINATES cancelRect = {69, 21, 126, 29};                    //cancel Cycle
 


//PRIMARY FONT
static const uint16_t Font5x7 [] = {
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // sp
0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000, 0x2000,   // !
0x5000, 0x5000, 0x5000, 0x0000, 0x0000, 0x0000, 0x0000,   // "
0x5000, 0x5000, 0xf800, 0x5000, 0xf800, 0x5000, 0x5000,   // #
0x2000, 0x7800, 0xa000, 0x7000, 0x2800, 0xf000, 0x2000,   // $
0xc000, 0xc800, 0x1000, 0x2000, 0x4000, 0x9800, 0x1800,   // %
0x4000, 0xa000, 0xa000, 0x4000, 0xa800, 0x9000, 0x6800,   // &
0x3000, 0x3000, 0x2000, 0x4000, 0x0000, 0x0000, 0x0000,   // '
0x1000, 0x2000, 0x4000, 0x4000, 0x4000, 0x2000, 0x1000,   // (
0x4000, 0x2000, 0x1000, 0x1000, 0x1000, 0x2000, 0x4000,   // )
0x6000, 0xf000, 0xf000, 0xf000, 0xf000, 0xf000, 0xf000,   // *
0x0000, 0x2000, 0x2000, 0xf800, 0x2000, 0x2000, 0x0000,   // +
0x0000, 0x0000, 0x0000, 0x0000, 0x3000, 0x3000, 0x2000,   // ,
0x0000, 0x0000, 0x0000, 0x7000, 0x0000, 0x0000, 0x0000,   // -
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3000, 0x3000,   // .
0x0000, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x0000,   // /
0x6000, 0x9000, 0x9000, 0xb000, 0xd000, 0x9000, 0x6000,   // 0
0x2000, 0x6000, 0x2000, 0x2000, 0x2000, 0x2000, 0xf000,   // 1
0x6000, 0x9000, 0x1000, 0x6000, 0x8000, 0x8000, 0xf000,   // 2
0xf000, 0x1000, 0x2000, 0x6000, 0x1000, 0x9000, 0x6000,   // 3
0x2000, 0x6000, 0xa000, 0xa000, 0xf000, 0x2000, 0x2000,   // 4
0x7000, 0x8000, 0xe000, 0x1000, 0x1000, 0x9000, 0x6000,   // 5
0x6000, 0x9000, 0x8000, 0xe000, 0x9000, 0x9000, 0x6000,   // 6
0xf000, 0x1000, 0x1000, 0x2000, 0x2000, 0x4000, 0x4000,   // 7
0x6000, 0x9000, 0x9000, 0x6000, 0x9000, 0x9000, 0x6000,   // 8
0x6000, 0x9000, 0x9000, 0x7000, 0x1000, 0x1000, 0x6000,   // 9
0x0000, 0x0000, 0x8000, 0x0000, 0x8000, 0x0000, 0x0000,   // :
0x0000, 0x0000, 0x2000, 0x0000, 0x2000, 0x2000, 0x4000,   // ;
0x0800, 0x1000, 0x2000, 0x4000, 0x2000, 0x1000, 0x0800,   // <
0x0000, 0x0000, 0xf800, 0x0000, 0xf800, 0x0000, 0x0000,   // =
0x4000, 0x2000, 0x1000, 0x0800, 0x1000, 0x2000, 0x4000,   // >
0x7000, 0x8800, 0x0800, 0x3000, 0x2000, 0x0000, 0x2000,   // ?
0x7000, 0x8800, 0xa800, 0xb800, 0xb000, 0x8000, 0x7800,   // @
0x6000, 0x9000, 0x9000, 0x9000, 0xf000, 0x9000, 0x9000,   // A
0xE000, 0x9000, 0x9000, 0xE000, 0x9000, 0x9000, 0xE000,   // B
0x6000, 0x9000, 0x8000, 0x8000, 0x8000, 0x9000, 0x6000,   // C
0xe000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0xe000,   // D
0xf000, 0x8000, 0x8000, 0xe000, 0x8000, 0x8000, 0xf000,   // E
0xf000, 0x8000, 0x8000, 0xe000, 0x8000, 0x8000, 0x8000,   // F
0x7000, 0x8000, 0x8000, 0xb000, 0x9000, 0x9000, 0x7000,   // G
0x9000, 0x9000, 0x9000, 0xf000, 0x9000, 0x9000, 0x9000,   // H
0xe000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0xe000,   // I
0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x9000, 0x6000,   // J
0x9000, 0x9000, 0xa000, 0xc000, 0xa000, 0x9000, 0x9000,   // K
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0xf000,   // L
0xf000, 0xa800, 0xa800, 0xa800, 0xa800, 0xa800, 0xa800,   // M
0xe000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000,   // N
0x6000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0x6000,   // O
0xe000, 0x9000, 0x9000, 0xe000, 0x8000, 0x8000, 0x8000,   // P
0x6000, 0x9000, 0x9000, 0x9000, 0x9000, 0xb000, 0x7000,   // Q
0xe000, 0x9000, 0x9000, 0xe000, 0x9000, 0x9000, 0x9000,   // R
0x7000, 0x8000, 0x8000, 0x6000, 0x1000, 0x1000, 0xe000,   // S
0xe000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,   // T
0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0xf000,   // U
0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0x6000,   // V
0xa800, 0xa800, 0xa800, 0xa800, 0xa800, 0xa800, 0x7000,   // W
0x9000, 0x9000, 0x9000, 0x6000, 0x9000, 0x9000, 0x9000,   // X
0x9000, 0x9000, 0x9000, 0x7000, 0x1000, 0x1000, 0xe000,   // Y
0xf000, 0x1000, 0x1000, 0x6000, 0x8000, 0x8000, 0xf000,   // Z
0x7800, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x7800,   // [
0x0000, 0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0000,   /* \ */
0x7800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x7800,   // ]
0x2000, 0x5000, 0x8800, 0x0000, 0x0000, 0x0000, 0x0000,   // ^
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xf800,   // _
0x6000, 0x6000, 0x2000, 0x1000, 0x0000, 0x0000, 0x0000,   // `
0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,   // a = Full Block 5x7
0xf800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0xf800,   // b = Half Block 5x7
0x0000, 0x0000, 0x8000, 0x8000, 0x8000, 0x0000, 0x0000,   // c = Battery Top
0x0800, 0x0800, 0x6800, 0x9800, 0x8800, 0x9800, 0x6800,   // d
0x0000, 0x0000, 0x7000, 0x8800, 0xf800, 0x8000, 0x7000,   // e
0x1000, 0x2800, 0x2000, 0x7000, 0x2000, 0x2000, 0x2000,   // f
0x0000, 0x0000, 0x7000, 0x9800, 0x9800, 0x6800, 0x0800,   // g
0x8000, 0x8000, 0xb000, 0xc800, 0x8800, 0x8800, 0x8800,   // h
0x2000, 0x0000, 0x6000, 0x2000, 0x2000, 0x2000, 0x7000,   // i
0x1000, 0x0000, 0x1000, 0x1000, 0x1000, 0x9000, 0x6000,   // j
0x8000, 0x8000, 0x9000, 0xa000, 0xc000, 0xa000, 0x9000,   // k
0x6000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x7000,   // l
0x0000, 0x0000, 0xd000, 0xa800, 0xa800, 0xa800, 0xa800,   // m
0x0000, 0x0000, 0xb000, 0xc800, 0x8800, 0x8800, 0x8800,   // n
0x0000, 0x0000, 0x7000, 0x8800, 0x8800, 0x8800, 0x7000,   // o
0x0000, 0x0000, 0xb000, 0xc800, 0xc800, 0xb000, 0x8000,   // p
0x0000, 0x0000, 0x6800, 0x9800, 0x9800, 0x6800, 0x0800,   // q
0x0000, 0x0000, 0xb000, 0xc800, 0x8000, 0x8000, 0x8000,   // r
0x0000, 0x0000, 0x7800, 0x8000, 0x7000, 0x0800, 0xf000,   // s
0x2000, 0x2000, 0xf800, 0x2000, 0x2000, 0x2800, 0x1000,   // t
0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x9800, 0x6800,   // u
0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x5000, 0x2000,   // v
0x0000, 0x0000, 0x8800, 0x8800, 0xa800, 0xa800, 0x5000,   // w
0x0000, 0x0000, 0x8800, 0x5000, 0x2000, 0x5000, 0x8800,   // x
0x0000, 0x0000, 0x8800, 0x8800, 0x7800, 0x0800, 0x8800,   // y
0x0000, 0x0000, 0xf800, 0x1000, 0x2000, 0x4000, 0xf800,   // z
0x1000, 0x2000, 0x2000, 0x4000, 0x2000, 0x2000, 0x1000,   // {
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,   // |
0x8000, 0x2000, 0x2000, 0x1000, 0x2000, 0x2000, 0x4000,   // }
0x4000, 0xa800, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000,   // ~
};

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
typedef struct {
	const uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef;

FontDef Font_6x7 = {6,7,Font5x7};
FontDef Font_5x7 = {5,7,Font5x7};
FontDef Font_4x7 = {4,7,Font5x7};
FontDef Font_2x7 = {2,7,Font5x7};

void ssd1305_WriteChar(FontDef Font, char ch, int color);
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
