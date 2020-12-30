#pragma once
#include <Arduino.h>

enum workType {
	POS,
	SPEED,
};

enum positioning {
	ABSOLUTE,
	RELATIVE,
};

class Stepper {
    public:
        // конструктор
        Stepper(int stepsPerRev, short pin1, short pin2);
        Stepper(int stepsPerRev, short pin1, short pin2, int ms1, int ms2, int ms3);
        // мотор движется к цели?
        bool tick();
        // инвертировать направление мотора
        void reverse(bool dir);
        // установка текущей позиции
        void setCurrent(long pos);
        void setCurrentDegrees(float pos);
        // получение текущей позиции
        long getCurrent();
        float getCurrentDegrees();

        // установка цели
        void setTarget(long pos, positioning type = ABSOLUTE);
        void setTargetDegrees(float pos, positioning type = ABSOLUTE);
        // получение цели
        long getTarget();
        float getTargetDegrees();

        // установка максимальной скорости
        void setMaxSpeed(float speed);
        void setMaxSpeedDegrees(float speed);

        // установка ускорения
        void setAcceleration(int accel);
        void setAccelerationDegrees(float accel);

        // остановка с ускорением
        void stop();

        // остановка без ускорения
        void full_stop();

        // reset
        void reset();

        // установка скорости
        void setSpeed(float speed);
        void setSpeedDegrees(float speed);
        // получение скорости
        float getSpeed();
        float getSpeedDegrees();

        // установка дробления шага
        short setMicrostepping(short mSteps);
        // получение текущего дробления шага
        short getMicrosteps();
        
        // установка режима работы
        void setWorkMode(workType mode);

        // период опрашивания 
        unsigned int stepTime = 10000;

    private:
        int sign(int x);
        void calcSpeed();
        void planner();

        unsigned int _plannerTime = 0;		
        int _plannerPrd = 15000;

        long _steps, _steps_original;
        float _accelTime = 0, _accelSpeed = 0;
        float _stepsPerDegrees;
        short _stepPin, _dirPin, _ms1Pin, _ms2Pin, _ms3Pin, _microstepping;
        unsigned int _prevTime = 0;			
        long _current = 0, _target = 0;

        short thisStep = 0;
        short _dir = 1;
        bool _isMoving = false;

        unsigned int DRIVER_STEP_TIME = 4;
        unsigned int _MIN_STEPPER_SPEED = 10;
        unsigned int _MAX_STEP_PERIOD = 1000000L/_MIN_STEPPER_SPEED;
        float MIN_STEPPER_SPEED = 1.0f/3600;
        float _maxSpeed = 300, _speed = 0;
        int _accel = 0;	
        float _accelInv = 0;

        workType _workMode = POS;
};
