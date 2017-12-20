#include "Arduino.h"

#ifndef __HorizonAccelerometer_h
#define __HorizonAccelerometer_h

#include "MPU60X0.h" // https://github.com/zrecore/freeIMU
// https://github.com/mjs513/MPU6050-Motion-Detection/blob/master/MPU6050_MotionDetect.ino

#define MPU_ADDRESS                   0x69
#define MOTION_DETECTION_DURATION_VALUE_SENSITIVE   5 // miliseconds
#define MOTION_DETECTION_THRESHOLD_VALUE_SENSITIVE  3
#define INT_MOTION_STATUS_BIT 0x40
#define INT_ZERO_MOTION_STATUS_BIT 0x20

namespace nmAccelerometer {
    class HorizonAccelerometer
    {
        uint8_t intStatus = 0;
        bool movementDetected = false;
        bool zeroMovementDetected = false;
        uint32_t lastMovementAt = 0;
        uint32_t lastStopAt = 0;
        bool accOk;
        
    public:
        HorizonAccelerometer();
        unsigned long secondsStopped(void);
        bool isStopped(void);
        bool isMoving(void);
        bool isStoppedInterval(void);
        void getInterruptStatus(void);
        bool setupAccelerometer(void);
        bool isMovementDetected(bool clear);
        bool isZeroMovementDetected(bool clear);
        
        uint32_t getLastStopAt() { return lastStopAt; }
        bool isOk() { return accOk; }
        
        
    };
}

#endif // def(__HorizonAccelerometer_h)
