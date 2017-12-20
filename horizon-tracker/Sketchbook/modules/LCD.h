#include <UTFT.h> // http://www.rinkydinkelectronics.com/resource/UTFT/UTFT.pdf

#ifndef __HorizonLCD_h
#define __HorizonLCD_h
#define LCD_MODEL ITDB18SP
#define LCD_SDA   35
#define LCD_SCL   34
#define LCD_CS    33
#define LCD_RST   37
#define LCD_RS    36

#define INITIAL_LINE 3

#define LCD_LINE_GAP        15
#define LCD_LINE_GAP_BIG    18
#define LCD_LINE_GAP_SMALL  11
#define LCD_SWITCH_PIN      25//26  // D26 - PA4 / ITDB02 Secondary interface pin BL  (PLACA V2 25//)

#define LCD_ON      LCDSwitch(ON);
#define LCD_OFF     LCDSwitch(OFF);
#define ORIENTATION LANDSCAPE

extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern unsigned int strider[2400];      // Strider Logo 40 x 46 pixes
extern unsigned int striderSmall[725];  // Strider Logo Small 10 x 12 pixels

bool isLcdOn(void);
void resetLcdTimeout(void);
void LCDclearScreen(void);
void LCDprint(const char * str, int pos, int line);
void LCDprint(String str, int pos, int line);
void LCDprintNumI(int num, int pos, int line, int length);
void LCDdrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                   int16_t w, int16_t h);
int LCDprintScreen(const char * originalStr, int pos, bool clearScreen, int lineGap);
int LCDprintScreen(String originalStr, int pos, bool clearScreen, int lineGap);
void LCDSwitch(uint8_t status);
void setupLCD(void);

extern UTFT utft;

#endif  // def(__HorizonLCD_h)
