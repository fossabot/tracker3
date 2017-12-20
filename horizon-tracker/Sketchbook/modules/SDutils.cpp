#include "SDutils.h"
#include "../Generic.h"

namespace nmSdCard {
    
    bool sdCardError = true;
    
    bool setupSDCard(void) {
        INFO;
        Terminal.print(F("Initializing SD card... "));
        LCDprint(F("SD Card..."), LEFT, currentLine += LCD_LINE_GAP);
        if(isSDPresent()) {
            Terminal.println(F("OK!"));
            LCDprint(F("OK!"), RIGHT, currentLine);
            return true;
        } else {
            Terminal.println(F("Card failed, or not present. Insert SD card and reset board..."));
            LCDprint(F("ERROR"), RIGHT, currentLine);
            return false;
        }
    }
    
    void printDirectory(HardwareSerial &serialPort, char * path, int numTabs) {
        File dir = SD.open(path);
        dir.rewindDirectory();
        while (true) {
            File entry =  dir.openNextFile();
            if(!entry) {
                break;  // no more files
            }
            for (uint8_t i = 0; i < numTabs; i++) {
                serialPort.print('\t');
            }
            //      Terminal.print(entry.name());
            if (entry.isDirectory()) {
                //        theSerial.print(entry.name());
                //        theSerial.println(F("/"));
                //        printDirectory(theSerial, entry, numTabs + 1);
            } else {
                serialPort.print(entry.name());
                // files have sizes, directories do not
                serialPort.print(F("\t\t"));
                serialPort.println(entry.size(), DEC);
            }
            entry.close();
        }
        dir.close();
    }
    
    void printDirectoryXbee(char *data, char * path, int numTabs) {
        char tmp[20];
        File dir = SD.open(path);
        dir.rewindDirectory();
        
        while (true) {
            File entry =  dir.openNextFile();
            if(!entry) {
                break;  // no more files
            }
            memset(tmp, 0, sizeof(tmp));
            for (uint8_t i = 0; i < numTabs; i++) {
                sprintf(tmp + strlen(tmp), "\t");
            }
            if (entry.isDirectory()) {
            } else {
                sprintf_P(tmp + strlen(tmp), PSTR("%s\t\t%lu"), entry.name(), entry.size());
            }
            entry.close();
            //        sendXbeeData(tmp, true);
        }
        dir.close();
    }
    
    bool isSDPresent(void) {
        if(sdCardError && SD.begin(SD_CS)) {
            sdCardError = false;
        }
        return !sdCardError;
    }
    
    bool dumpFile(HardwareSerial &serialPort, char * fileName, bool delayBetweenLines) {
        File dataFile = SD.open(fileName);
        if(dataFile) {
            while(dataFile.available()) {
                byte data = dataFile.read();
                serialPort.write(data);
                if(delayBetweenLines && data == CR) {
                    delay(100);
                }
            }
            dataFile.close();
            return true;
        }
        return false;
    }
    
    bool writeSDline(char *logFileName, char *text) {
        bool success = false;
        int attempt;
        if(!sdCardError){
            delay(50);
            File file = SD.open(logFileName, FILE_WRITE);
            if(file){
                file.write(text, strlen(text));
                file.close();
                success = true;
            }else {
                //if(SHOW_ERROR){ERROR;
                Terminal.println(F("writeSDline: error opening log file"));//}
                sdCardError = true;
                delay(100);
                isSDPresent();
                Terminal.println(sdCardError);
            }
        } else {
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("INSERT SD CARD"));}
            sdCardError = true;
            delay(100);
            isSDPresent();
        }
        return success;
    }
    
    bool overwriteSDline(char *logFileName, char *text) {
        bool success = false;
        if(!sdCardError){
            remove(logFileName);
            File file = SD.open(logFileName, FILE_WRITE);
            file.seek(0);
            if(file){
                file.write(text, strlen(text));
                file.close();
                success = true;
            }else {
                if(SHOW_ERROR){ERROR;
                    Terminal.println(F("overwriteSDline: error opening log file"));}
                sdCardError = true;
                delay(100);
                isSDPresent();
                Terminal.println(sdCardError);
            }
        } else {
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("INSERT SD CARD"));}
            sdCardError = true;
            delay(100);
            isSDPresent();
        }
        return success;
    }
    
    bool writeSDline(String logFileName, char *text) {
        char buf[logFileName.length() + 1];
        logFileName.toCharArray(buf, logFileName.length() + 1);
        return writeSDline(buf, text);
    }
    
    bool readNextLog(char *logFileName, char *myLine){
        bool success = false;
        if(!sdCardError) {
            File dir = SD.open(LOG_DIR);
            dir.rewindDirectory();
            File file =  dir.openNextFile();
            while(file.isDirectory()){
                file.close();
                file = dir.openNextFile();
            }
            memcpy(logFileName, file.name(), String(file.name()).length());
            if(file) {
                if(file.available()){
                    String readLine = file.readStringUntil('\n');
                    readLine.concat("\n");
                    if(readLine.length()<128)
                    {
                        memcpy(myLine,readLine.c_str(),readLine.length());
                        if(file.available()){
                            if(SHOW_ERROR){ERROR;
                                Terminal.println(F("SD: readNextLog: too much data in file"));}
                            success = false;
                        }else{
                            success = true;
                        }
                    }else{
                        Terminal.println(F("SD: readNextLog: invalid file"));
                        Terminal.println(logFileName);
                        file.close();
                        dir.close();
                        char filePath[24];
                        memset(filePath, 0, sizeof(filePath));
                        sprintf_P(filePath + strlen(filePath), PSTR("%s/%s"),LOG_DIR,logFileName);
                        remove(filePath);
                        return false;
                    }
                }else{
                    if(SHOW_INFO){INFO;
                        Terminal.println(F("readLine: NO OFFLINE DATA TO SEND"));}
                    Terminal.println(F("SD: readNextLog: invalid file"));
                    Terminal.println(logFileName);
                    file.close();
                    dir.close();
                    char filePath[24];
                    memset(filePath, 0, sizeof(filePath));
                    sprintf_P(filePath + strlen(filePath), PSTR("%s/%s"),LOG_DIR,logFileName);
                    remove(filePath);
                    return false;
                }
            } else {
                if(SHOW_ERROR){ERROR;
                    Terminal.println(F("readLine: error opening log file"));}
                sdCardError = true;
                delay(100);
                isSDPresent();
            }
            file.close();
            dir.close();
        } else {
            if(SHOW_ERROR){ERROR;
            Terminal.println(F("readLine: INSERT SD CARD"));}
            sdCardError = true;
            delay(100);
            isSDPresent();
        }
        
        return success;
    }
    
    
    bool readLine(const char * logFileName, char *myLine){
        bool success = false;
        if(!sdCardError) {
            File file = SD.open(logFileName, FILE_READ);
            if(file) {
                if(file.available()){
                    String readLine = file.readStringUntil('\r');
                    file.close();
                    memcpy(myLine,readLine.c_str(),readLine.length());
                    success = true;
                }else{
                    file.close();
                    if(SHOW_INFO){INFO;
                        Terminal.println(F("readLine: NO OFFLINE DATA TO SEND"));}
                }
            } else {
                if(SHOW_ERROR){ERROR;
                Terminal.println(F("readLine: error opening log file"));}
                sdCardError = true;
                delay(100);
                isSDPresent();
            }
        } else {
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("readLine: INSERT SD CARD"));}
            sdCardError = true;
            delay(100);
            isSDPresent();
        }
        return success;
    }
    
    bool exists(const char *fileName){
        return SD.exists(fileName);
    }
    
    bool remove(const char *fileName){
        return SD.remove(fileName);
    }
    
    bool isEmpty(const char *path){
        File dir = SD.open(path);
        dir.rewindDirectory();
        bool empty = false;
        File entry =  dir.openNextFile();
        if(!entry) {
            empty = true;
        }
        entry.close();
        dir.close();
        return empty;
    }
    
    bool mkDir(const char *path){
        return SD.mkdir(path);
    }
    
    bool rmDir(const char *path){
        File dir = SD.open(LOG_DIR);
        dir.rewindDirectory();
        while (true) {
            File entry = dir.openNextFile();
            while(entry.isDirectory()){
                entry.close();
                entry = dir.openNextFile();
            }
            Terminal.print("dir: ");
            Terminal.println(dir.name());
            Terminal.print("file: ");
            Terminal.println(entry.name());
            if(!entry) {
                break;  // no more files
            }
            char filePath[24];
            memset(filePath, 0, sizeof(filePath));
            sprintf_P(filePath + strlen(filePath), PSTR("%s/%s"),LOG_DIR,entry.name());
            if(!SD.remove(filePath)){
                Terminal.print("Could not remove file: ");
                Terminal.println(filePath);
            }
            
        }
        dir.close();
    }
    
    bool isOk(){
        return !sdCardError;
    }
    
    uint32_t fileSize(const char *path){
        File file = SD.open(path);
        return file.size();
    }
    
}
