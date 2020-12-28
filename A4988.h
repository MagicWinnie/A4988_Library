#pragma once
#include <Arduino.h>

enum coordSystem {
    ABSOLUTE = 0,
    RELATIVE = 1
};

enum workMode {
    POSITIONING = 0,
    ROTATE = 1
};

class Stepper {
    public:
        Stepper(int steps, int stepPin, int dirPin, int ms0Pin = -1, int ms1Pin = -1, int ms2Pin = -1);
        void microStepping(int mSteps);

        void setGoalSteps(long goalSteps, int mode);
        void setGoalDeg(float pos, coordSystem mode = ABSOLUTE);
        void setGoalCm();

        void stop();
        void fullStop();

        void changeDir(bool dir);
        void reset();
        
        bool done();

        int getMicrosteps();
        long getCurrentPoseSteps();
        float getCurrentPoseDeg();

        unsigned int delayStep;
    private:
        int sign(int x);
        int signZ(int x);
        bool _isA4988 = false;
        bool _isTMC2208 = false;
        bool _isMoving = false;
        bool _workType = 0;

        int _steps, _steps_original;
        int _microstepping, _dir; // _dir = {-1, 1}
        long _current, _target;
        float _steps_per_deg;

        // pins
        int _stepPin, _dirPin;
        int _ms0Pin, _ms1Pin, _ms2Pin;
};