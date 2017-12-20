#include "Functions.h"
#include "../Generic.h"
/*---------------------------------------------------------------------------*/
/**
 * Generic functions
 */
/*---------------------------------------------------------------------------*/
void switchLed(void) {
    digitalWrite(LED, !digitalRead(LED));
}

void softwareReset() { // Restarts program from beginning but does not reset the peripherals and registers
    asm volatile ("  jmp 0");
}

void longToByteArray(uint32_t origin, char * dest) {
    byte i = 24;
    for(byte j = 0; j < 4; j++) {
        dest[j] = (origin >> i) & 0x000000FF;
        i -= 8;
    }
}

uint8_t checksum(char * buffer, uint16_t count) {
    byte sum = 0;
    byte result = 0;
    for(int i = 0; i < count; i++) {
        sum += buffer[i];
    }
    return 0xFF - sum;
}

uint16_t calcCRC(uint8_t *packet, uint8_t u8length) {
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    for (unsigned char i = 0; i < u8length; i++) {
        temp = temp ^ packet[i];
        for (unsigned char j = 1; j <= 8; j++) {
            flag = temp & 0x0001;
            temp >>=1;
            if(flag) {
                temp ^= 0xA001;
            }
        }
    }
    // Reverse byte order.
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    // the returned value is already swapped
    // crcLo byte is first & crcHi byte is last
    return temp;
}

#ifdef SDCARD
uint32_t fileCRC32(File &file, uint32_t &charcnt) {
    unsigned long crc = ~0L;
    file.seek(0);
    while(file.available()) {
        crc = CRC32::update(crc, file.read());
        charcnt++;
    }
    return ~crc;
}
uint32_t fileCRC32fixedLength(File &file, uint32_t charcnt) {
    unsigned long crc = ~0L;
    uint32_t i = 0;
    file.seek(0);
    while(file.available() && i++ < charcnt) {
        crc = CRC32::update(crc, file.read());
    }
    return ~crc;
}
#endif

/* Timeout calculation
 * Return true when
 * -  lastTimeAt not set yet (first execution)
 * or lastTimeAt bigger then millis() (millis resets after ~50 days)
 * or timeout really ocurred (millis() - lastTimeAt is bigger then period it should timeout)
 */
bool timedOut(uint32_t lastTimeAt, uint32_t periodInSeconds) {
    return (!lastTimeAt || lastTimeAt > millis() || ((millis() - lastTimeAt) >= (periodInSeconds * 1000)));
}
bool timedOutMs(uint32_t lastTimeAt, uint32_t periodInMiliseconds) {
    return (!lastTimeAt || lastTimeAt > millis() || ((millis() - lastTimeAt) >= periodInMiliseconds));
}

//String int16ToStrFloat(int16_t value) {
//  return int16ToStrFloat(value, 1, 2);
//}

String int16ToStrFloat(int16_t value, int precision, int decimal) {
    char ta[12]; // Temp array
    return dtostrf((float) value/100, precision, decimal, ta);
}

//String floatToStrFloat(float value) {
//  return floatToStrFloat(value, 1, 1);
//}

String floatToStrFloat(float value, int precision, int decimal) {
    char ta[12]; // Temp array
    return dtostrf(value, precision, decimal, ta);
}

String int32ToStrFloat(int32_t value) {
    char ta[12]; // Temp array
    return dtostrf((float) value/100, 2, 2, ta);
}

String ucharToStr(unsigned char *value, int length){
    
}

bool stoba(short origin, byte *dest){
    if(origin == NULL){
        return false;
    }
    byte *b = (byte *)&origin;
    for(byte j = 0; j < sizeof(short); j++) {
        dest[j] = b[sizeof(short)-1-j];
    }return true;
}

bool itoba(int32_t origin, byte *dest){
    if(origin == NULL){
        return false;
    }
    byte *b = (byte *)&origin;
    for(byte j = 0; j < sizeof(int32_t); j++) {
        dest[j] = b[sizeof(int32_t)-1-j];
    }return true;
}

bool dtoba(double origin, byte *dest){
    if(origin == NULL){
        return false;
    }
    byte *b = (byte *)&origin;
    for(byte j = 0; j < sizeof(double); j++) {
        dest[j] = b[sizeof(double)-1-j];
    }return true;
}

bool ltoba(long origin, byte *dest){
    if(origin == NULL){
        return false;
    }
    byte *b = (byte *)&origin;
    for(byte j = 0; j < sizeof(long); j++) {
        dest[j] = b[sizeof(long)-1-j];
    }return true;
}

void toHexaString(char *dest, char *data, int size){
    memset(dest, 0, (size*2)+1);
    for(int a = 0 ; a < size; a++){
        sprintf(dest + strlen(dest), "%02X",byte(data[a]));
    }
}

void printAsHexa(char *data, int size){
    char dataString[256] = {0};
    for(int a = 0 ; a < size; a++){
        sprintf(dataString + strlen(dataString), "%02X",byte(data[a]));
    }
    Terminal.println(dataString);
}

int MonitorData::toCharArray(char *data){
    
    memset(data, 0, sizeof(data));
    sprintf_P(data + strlen(data), PSTR("%c,%04d,"), stationType, firmwareVersion);
    memcpy(data + strlen(data), bluetoothSerial, 12);
    sprintf_P(data + strlen(data), PSTR(","));
    
    char temp[12];
    memset(temp, 0, sizeof(temp));
    dtostrf(lat/ 10000000.0, 1, 6, temp);
    sprintf_P(data + strlen(data), PSTR("%s,"),temp);
    memset(temp, 0, sizeof(temp));
    dtostrf(lon/ 10000000.0, 1, 6, temp);
    sprintf_P(data + strlen(data), PSTR("%s,"),temp);
    
    
    sprintf_P(data + strlen(data), PSTR("%s,"), gpsStatus & GNSS_FIX ? "VALID" : "INVALID");
    
    //WRITE GPS DATE
    sprintf_P(data + strlen(data), PSTR("%02d%02d%04d,"), day, month, year);
    sprintf_P(data + strlen(data), PSTR("%02d%02d%02d,"), hour, min, sec);
    
    //WRITE SPEED
    memset(temp, 0, sizeof(temp));
    dtostrf(gSpeed* 0.0036, 1, 2, temp);
    sprintf_P(data + strlen(data), PSTR("%s,"),temp);
    
    //WRITE COURSE
    memset(temp, 0, sizeof(temp));
    dtostrf(headMot/ 100000.0, 1, 0, temp);
    sprintf_P(data + strlen(data), PSTR("%s,"),temp);
    //WRITE ALTITUDE
    memset(temp, 0, sizeof(temp));
    dtostrf(alt/ 1000.0, 1, 0, temp);
    sprintf_P(data + strlen(data), PSTR("%s,"),temp);
    
    //WRITE SATTELITTES and HDOP
    sprintf_P(data + strlen(data), PSTR("%02d,"), systemStatus);
    
    memset(temp, 0, sizeof(temp));
    dtostrf(hAcc/ 1000.0, 1, 0, temp);
    sprintf_P(data + strlen(data), PSTR("%s,"),temp);
    
    switch (accelerometerStatus) {
        case 'I':
            sprintf(data + strlen(data), "INIT,");
            break;
        case 'R':
            sprintf(data + strlen(data), "ERROR,");
            break;
        case 'B':
            sprintf(data + strlen(data), "MOVE_START,");
            break;
        case 'E':
            sprintf(data + strlen(data), "MOVE_END,");
            break;
        case 'S':
            sprintf(data + strlen(data), "STOP,");
            break;
        case 'M':
            sprintf(data + strlen(data), "MOVE,");
            break;
        case 'U':
            sprintf(data + strlen(data), "UNKNOWN,");
        default:
            break;
    }

    sprintf_P(data + strlen(data), PSTR("%d\r\n"), horizonXbee.isOnline() ? horizonXbee.getPowerLevel() : 0);
    return 0;
}

int MonitorData::toByteArray(char *data){
    return 0;
}

void MonitorData::print(){
    Terminal.println("----------------Monitor data----------------");
    Terminal.print("stationType = "); Terminal.println(stationType);
    Terminal.print("firmwareVersion = "); Terminal.println(firmwareVersion);
    Terminal.print("bluetoothSerial = "); Terminal.println(bluetoothSerial);
    Terminal.print("accelerometerStatus = "); Terminal.println(accelerometerStatus);
    Terminal.print("signalPower = "); Terminal.println(signalPower);
    Terminal.print("systemStatus = "); Terminal.println(systemStatus,BIN);
    Terminal.print("batteryLevel = "); Terminal.println(batteryLevel);
    Terminal.print("gpsStatus = "); Terminal.println(gpsStatus,BIN);
    Terminal.print("year = "); Terminal.println(year);
    Terminal.print("month = "); Terminal.println(month);
    Terminal.print("day = "); Terminal.println(day);
    Terminal.print("hour = "); Terminal.println(hour);
    Terminal.print("min = "); Terminal.println(min);
    Terminal.print("sec = "); Terminal.println(sec);
    Terminal.print("gpsFixType = "); Terminal.println(gpsFixType);
    Terminal.print("lon = "); Terminal.println(lon/ 10000000.0, 7);
    Terminal.print("lat = "); Terminal.println(lat/ 10000000.0, 7);
    Terminal.print("alt(m) = "); Terminal.println(alt/ 1000.0, 3);
    Terminal.print("headMot(deg) = "); Terminal.println(headMot/ 100000.0, 5);
    Terminal.print("hAcc(m) = "); Terminal.println(hAcc/ 1000.0, 3);
    Terminal.print("vAcc(m) = "); Terminal.println(vAcc/ 1000.0, 3);
    Terminal.print("gSpeed(km/h) = "); Terminal.println(gSpeed* 0.0036, 5);
    Terminal.print("sAcc(km/h) = "); Terminal.println(sAcc* 0.0036, 5);
    Terminal.print("headVeh(deg) = "); Terminal.println(headVeh/ 100000.0, 5);
    Terminal.print("magDec(deg) = "); Terminal.println(magDec/ 100.0, 2);
    Terminal.print("magAcc(deg) = "); Terminal.println(magAcc/ 100.0, 2);
}

void MonitorData::clear(){
    
}

bool MonitorData::update(){
    if(justStarted){
        this->stationType = 'T';
        this->firmwareVersion = SW_TRACKER_VERSION;
        memcpy(this->bluetoothSerial, horizonBluetooth.getSerial(), 12);
        this->accelerometerStatus = 'I';
    }else{
        if(!horizonAccelerometer.isOk()) this->accelerometerStatus = 'R';
        else if(motionStart) this->accelerometerStatus = 'B';
        else if(motionEnded) this->accelerometerStatus = 'E';
        else if(horizonAccelerometer.isStopped()) this->accelerometerStatus = 'S';
        else if(!horizonAccelerometer.isStopped()) this->accelerometerStatus = 'M';
        else this->accelerometerStatus = 'U';
    }
    this->signalPower = horizonXbee.getPowerLevel();
    
    this->systemStatus = 0x00;
    if(nmSdCard::isOk()) this->systemStatus = this->systemStatus | SD_OK;
    else this->systemStatus = this->systemStatus & SD_ERROR;
    
    if(bluetoothConnected) this->systemStatus = this->systemStatus | BT_CON;
    else this->systemStatus = this->systemStatus & BT_NCON;
    
    if(horizonXbee.isOnline()) this->systemStatus = this->systemStatus | XB_CON;
    else this->systemStatus = this->systemStatus & XB_NCON;
    
    if(horizonConnected) this->systemStatus = this->systemStatus | HR_CON;
    else this->systemStatus = this->systemStatus & HR_NCON;
    
    if(!nmSdCard::isEmpty(LOG_DIR)) this->systemStatus = this->systemStatus | OFF_DATA;
    else this->systemStatus = this->systemStatus & NO_OFF_DATA;
    
    if(logSaved) this->systemStatus = this->systemStatus | LOG_SAVED;
    else this->systemStatus = this->systemStatus & LOG_FAIL;
    
    this->batteryLevel = batteryLevel;
    if(gpsAntennaRemoved){
        this->gpsStatus = 0x00;
        gpsAntennaRemoved = false;
    }
}

bool MonitorData::updateGpsData(){
    this->gpsStatus = 0x00;
    this->gpsStatus | UBX;
    this->gpsStatus = this->gpsStatus | ANT_OK;
    this->gpsStatus = this->gpsStatus | ((horizonGPS.rawPacket[11 + 4] << 2) & TIME_VALID); //Time and Date valid
    this->gpsStatus = this->gpsStatus | ((horizonGPS.rawPacket[21 + 4] << 4) & GNSS_FIX); //Gnss fix ok
    this->gpsStatus = this->gpsStatus | (horizonGPS.rawPacket[21 + 4] & HEAD_VEH); //head veh
    memcpy(&this->year, &horizonGPS.rawPacket[4 + 4], 2);
    this->month = horizonGPS.rawPacket[6 + 4];
    this->day = horizonGPS.rawPacket[7 + 4];
    this->hour = horizonGPS.rawPacket[8 + 4];
    this->min = horizonGPS.rawPacket[9 + 4];
    this->sec = horizonGPS.rawPacket[10 + 4];
    this->gpsFixType = horizonGPS.rawPacket[20 + 4];
    memcpy(&this->lon, &horizonGPS.rawPacket[24 + 4], 4);
    memcpy(&this->lat, &horizonGPS.rawPacket[28 + 4], 4);
    memcpy(&this->alt, &horizonGPS.rawPacket[32 + 4], 4);
    memcpy(&this->headMot, &horizonGPS.rawPacket[64 + 4], 4);
    memcpy(&this->hAcc, &horizonGPS.rawPacket[40 + 4], 4);
    memcpy(&this->vAcc, &horizonGPS.rawPacket[44 + 4], 4);
    memcpy(&this->gSpeed, &horizonGPS.rawPacket[60 + 4], 4);
    memcpy(&this->sAcc, &horizonGPS.rawPacket[68 + 4], 4);
    memcpy(&this->headVeh, &horizonGPS.rawPacket[84 + 4], 4);
    memcpy(&this->magDec, &horizonGPS.rawPacket[88 + 4], 2);
    memcpy(&this->magAcc, &horizonGPS.rawPacket[90 + 4], 2);
}

bool MonitorData::updateCalibStatus(){
    this->fusionMode = horizonGPS.rawPacket[12 + 4];
}

/*---------------------------------------------------------------------------*/

///*---------------------------------------------------------------------------*/
///**
// * I2C Auxiliary functions
// */
///*---------------------------------------------------------------------------*/
//#ifdef ACCELEROMETER
//  void i2cWriteByte(uint8_t address, uint8_t subAddress, uint8_t data) {
//    Wire.beginTransmission(address);  // Initialize the Tx buffer
//    Wire.write(subAddress);           // Put slave register address in Tx buffer
//    Wire.write(data);                 // Put data in Tx buffer
//    Wire.endTransmission();           // Send the Tx buffer
//  }
//
//  uint8_t i2cReadByte(uint8_t address, uint8_t subAddress) {
//    uint8_t data; // `data` will store the register data
//    Wire.beginTransmission(address);         // Initialize the Tx buffer
//    Wire.write(subAddress);                  // Put slave register address in Tx buffer
//    Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
//    Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address
//    data = Wire.read();                      // Fill Rx buffer with result
//    return data;                             // Return data read from slave register
//  }
//
//  void i2cReadBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest) {
//    Wire.beginTransmission(address);   // Initialize the Tx buffer
//    Wire.write(subAddress);            // Put slave register address in Tx buffer
//    Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
//    uint8_t i = 0;
//    Wire.requestFrom(address, count);  // Read bytes from slave register address
//    while (Wire.available()) {
//      dest[i++] = Wire.read();
//    }         // Put read results in the Rx buffer
//  }
//#endif
///*---------------------------------------------------------------------------*/
