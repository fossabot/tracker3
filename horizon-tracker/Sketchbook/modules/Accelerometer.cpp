#include "Accelerometer.h"
#include "../Generic.h"

namespace nmAccelerometer {
    MPU60X0 accelgyro(false, MPU_ADDRESS);
    
    HorizonAccelerometer::HorizonAccelerometer()
    :intStatus(0)
    ,movementDetected(false)
    ,zeroMovementDetected(false)
    ,lastMovementAt(0)
    ,lastStopAt(0)
    ,accOk(false)
    {
    }
    
    
    bool HorizonAccelerometer::setupAccelerometer(void) {
        INFO;
        Terminal.print(F("Initializing Accelerometer... "));
        LCDprint(F("Acceler..."), LEFT, currentLine += LCD_LINE_GAP);
        Wire.begin();
        accelgyro.initialize();
        accelgyro.setAccelerometerPowerOnDelay(3);
        accelgyro.setIntZeroMotionEnabled(0);
        accelgyro.setDHPFMode(1);
        accelgyro.setMotionDetectionThreshold(MOTION_DETECTION_THRESHOLD_VALUE_SENSITIVE);
        accelgyro.setMotionDetectionDuration(MOTION_DETECTION_DURATION_VALUE_SENSITIVE);
        accelgyro.setIntMotionEnabled(true);
        accelgyro.setZeroMotionDetectionThreshold(MOTION_DETECTION_THRESHOLD_VALUE_SENSITIVE);
        accelgyro.setZeroMotionDetectionDuration(MOTION_DETECTION_DURATION_VALUE_SENSITIVE);
        accelgyro.setIntZeroMotionEnabled(true);
        
        if(accelgyro.testConnection()) {
            Terminal.println(F("OK!"));
            LCDprint(F("OK!"), RIGHT, currentLine);
            accOk = true;
        } else {
            Terminal.println(F("ERROR"));
            LCDprint(F("ERROR"), RIGHT, currentLine);
            accOk = false;
        }
        return accOk;
    }
    
    unsigned long HorizonAccelerometer::secondsStopped(void) {
        return (millis() - lastMovementAt) / 1000;
    }
    
    bool HorizonAccelerometer::isStopped(void) {
        return lastStopAt + STOP_TIMEOUT*1000 > lastMovementAt;
    }
    
    bool HorizonAccelerometer::isMoving(void) {
        return !isStopped();
    }
    
    bool HorizonAccelerometer::isStoppedInterval(void) {
        return (secondsStopped() % STOP_TRACK_INTERVAL == 0);
    }
    
    void HorizonAccelerometer::getInterruptStatus(void){
        intStatus=accelgyro.getIntStatus();
        movementDetected = intStatus&INT_MOTION_STATUS_BIT;
        zeroMovementDetected = intStatus&INT_ZERO_MOTION_STATUS_BIT;
        if(movementDetected){
            if(lastMovementAt != millis()) {
                lastMovementAt = millis();
            }
        }
        if(zeroMovementDetected){
            if(lastStopAt != millis()) {
                lastStopAt = millis();
            }
        }
    }
    
    
    bool HorizonAccelerometer::isMovementDetected(bool clear) {
        if(movementDetected && clear) {
            movementDetected = false;
            return true;
        }
        return movementDetected;
    }
    
    bool HorizonAccelerometer::isZeroMovementDetected(bool clear) {
        if(zeroMovementDetected && clear) {
            zeroMovementDetected = false;
            return true;
        }
        return zeroMovementDetected;
    }
    
    
}
