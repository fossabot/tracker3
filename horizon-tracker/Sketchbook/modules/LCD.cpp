#include "LCD.h"
#include "../Generic.h"

UTFT utft(LCD_MODEL, LCD_SDA, LCD_SCL, LCD_CS, LCD_RST, LCD_RS);

bool isLcdOn(void) {
    return digitalRead(LCD_SWITCH_PIN) == HIGH;
}

void resetLcdTimeout(void) {
    lcdOnAt = millis();
}

void LCDclearScreen(void) {
    if(isLcdOn()) {
        utft.clrScr();
    }
    currentLine = INITIAL_LINE;
}

void LCDprint(const char * str, int pos, int line) {
    if(isLcdOn()) {
        utft.print(str, pos, line);
    }
}

void LCDprint(String str, int pos, int line) {
    char buf[str.length() + 1];
    str.toCharArray(buf, str.length() + 1);
    LCDprint(buf, pos, line);
}

void LCDprintNumI(int num, int pos, int line, int length) {
    if(isLcdOn()) {
        utft.printNumI(num, pos, line, length);
    }
}

void LCDdrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                   int16_t w, int16_t h){
    utft.drawBitmap(x,y,bitmap,w,h);
}

int LCDprintScreen(const char * originalStr, int pos, bool clearScreen, int lineGap) {
    char* str = strdup(originalStr); // Create a writable copy of the string so we can tokenize it
    int lineToPrint = INITIAL_LINE;
    if(isLcdOn()) {
        if(clearScreen) {
            LCDclearScreen();
        }
        char *tmp = strtok(str, "\n"); // 0x0A
        while(tmp) {
            LCDprint(tmp, pos, lineToPrint);
            lineToPrint += lineGap;
            tmp = strtok(NULL, "\n");
        }
        free(tmp);
    }
    free(str);
    return lineToPrint;
}

int LCDprintScreen(String originalStr, int pos, bool clearScreen, int lineGap) {
    char buf[originalStr.length() + 1];
    originalStr.toCharArray(buf, originalStr.length() + 1);
    return LCDprintScreen(buf, pos, clearScreen, lineGap);
}

void LCDSwitch(uint8_t status) {
    if(status == OFF) {
        digitalWrite(LCD_SWITCH_PIN, status);
        pinMode(LCD_SDA, INPUT);
        pinMode(LCD_SCL, INPUT);
        pinMode(LCD_CS, INPUT);
        pinMode(LCD_RST, INPUT);
        pinMode(LCD_RS, INPUT);
        lcdOffAt = millis();
        if(SHOW_INFO){INFO;
            Terminal.println(F("------------------------------- LCD OFF -------------------------------"));}
    } else {
        if(!isLcdOn()) {
            digitalWrite(LCD_SWITCH_PIN, status);
            utft.InitLCD(ORIENTATION);
            utft.setFont(SmallFont);
        }
        utft.clrScr();
        resetLcdTimeout();
        currentLine = INITIAL_LINE;
        if(SHOW_INFO){INFO;
            Terminal.println(F("------------------------------- LCD ON --------------------------------"));}
    }
}

void setupLCD(void) {
    pinMode(LCD_SWITCH_PIN, OUTPUT);
    LCD_OFF;
    LCD_ON;
    INFO;
    Terminal.println(F("Initializing LCD... OK!"));
    
    if(ORIENTATION == PORTRAIT)
        utft.drawBitmap(45, 60, 40, 46, strider); // Print Strider Logo // drawBitmap (x, y, sx, sy, data[, scale]);
    else
        utft.drawBitmap(60, 47, 40, 46, strider); // Print Strider Logo // drawBitmap (x, y, sx, sy, data[, scale]);
    
    utft.setFont(BigFont);
    
    LCDprint(F("STRIDER"), CENTER, 5);
    LCDprint(F("HORIZON"), CENTER, 25);
    LCDprint(F("TRACKER"), CENTER, 100);
    utft.setFont(SmallFont);
    delay(3000);
    utft.clrScr();
}
