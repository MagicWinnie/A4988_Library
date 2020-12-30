#include "A4988.h"

Stepper::Stepper(int stepsPerRev, short pin1, short pin2)
{
    _stepPin = pin1;
    _dirPin = pin2;

    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);
    
    _stepsPerDegrees = stepsPerRev / 360.0;
    _steps = stepsPerRev;
    _steps_original = stepsPerRev;
    
    setMaxSpeed(300);
    setAcceleration(300);
}

Stepper::Stepper(int stepsPerRev, short pin1, short pin2, int ms1, int ms2, int ms3)
{
    _stepPin = pin1;
    _dirPin = pin2;
    _ms1Pin = ms1;
    _ms2Pin = ms2;
    _ms3Pin = ms3;

    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);
    
    _stepsPerDegrees = stepsPerRev / 360.0;
    _steps = stepsPerRev;
    _steps_original = stepsPerRev;
    
    pinMode(ms1, OUTPUT); pinMode(ms2, OUTPUT); pinMode(ms3, OUTPUT);
    _microstepping = 1;

    setMaxSpeed(300);
    setAcceleration(300);
}

bool Stepper::tick() {
    if (_isMoving && !_workMode && _accel != 0 && _maxSpeed > _MIN_STEPPER_SPEED) planner();	
    
    if (_isMoving && micros() - _prevTime >= stepTime) {
        _prevTime = micros();	
        if (!_workMode && _target == _current) {
            full_stop();
            return false;					
        }
        
        _current += _dir;
        digitalWrite(_dirPin, (_dir > 0 ? _dir : 0));
        digitalWrite(_stepPin, 1);
        delayMicroseconds(DRIVER_STEP_TIME);
        digitalWrite(_stepPin, 0);
        delayMicroseconds(DRIVER_STEP_TIME);
    }		
    return _isMoving;
}

void Stepper::setTarget(long pos, positioning type = ABSOLUTE) {
    _target = type ? (_current + pos) : pos;		
    if (_target != _current) {
        calcSpeed(); 
        _isMoving = true;
    }
}

short Stepper::setMicrostepping(short mSteps)
{
    if (_microstepping == -1)
        return -1;

    switch (mSteps)
    {
    case 1:
        _steps = _steps_original;
        digitalWrite(_ms1Pin, LOW); digitalWrite(_ms2Pin, LOW); digitalWrite(_ms3Pin, LOW);
        break;
    case 2:
        _steps = _steps_original * 2;
        digitalWrite(_ms1Pin, LOW); digitalWrite(_ms2Pin, LOW); digitalWrite(_ms3Pin, HIGH);
        break;
    case 4:
        _steps = _steps_original * 4;
        digitalWrite(_ms1Pin, LOW); digitalWrite(_ms2Pin, HIGH); digitalWrite(_ms3Pin, LOW);
        break;
    case 8:
        _steps = _steps_original * 8;
        digitalWrite(_ms1Pin, LOW); digitalWrite(_ms2Pin, HIGH); digitalWrite(_ms3Pin, HIGH);
        break;
    case 16:
        _steps = _steps_original * 16;
        digitalWrite(_ms1Pin, HIGH); digitalWrite(_ms2Pin, HIGH); digitalWrite(_ms3Pin, HIGH);
        break;
    default:
        return -1;
        break;
    }

    _microstepping = mSteps;
    return mSteps;
}

void Stepper::setMaxSpeed(float speed) {
    _maxSpeed = max(speed, MIN_STEPPER_SPEED);
    calcSpeed();
    _plannerPrd = map((int)_maxSpeed, 1000, 20000, 15000, 1000);
    _plannerPrd = constrain(_plannerPrd, 15000, 1000);	
}

void Stepper::setAcceleration(int accel) {
    _accel = accel; 
    _accelInv = 0.5f / accel;
    _accelTime = accel / 1000000.0f;
}

void Stepper::stop() {
    if (_isMoving) {
        if (_workMode == POS) {
            _accelSpeed = 1000000.0f / stepTime * _dir;
            setTarget(_current + (float)_accelSpeed * _accelSpeed * _accelInv * _dir);
            setMaxSpeed(abs(_accelSpeed));
        } else {
            setSpeed(0);
        }
    }
}

void Stepper::full_stop() {
    if (_isMoving) {
        _isMoving = false;
        _accelSpeed = 0;
    }
}

void Stepper::setSpeed(float speed) {
    _speed = speed;
    if (abs(_speed) < MIN_STEPPER_SPEED) _speed = MIN_STEPPER_SPEED * sign(_speed);
    
    if (_speed == 0) { full_stop(); return; }
    _accelSpeed = _speed;
    stepTime = 1000000.0 / abs(_speed);
    _dir = (_speed > 0) ? 1 : -1;	
    
    _isMoving = true;
}

void Stepper::setWorkMode(workType mode){
    _workMode = mode; 
    if (mode == SPEED) calcSpeed();
}

void Stepper::calcSpeed() {
    if (!_workMode && (_accel == 0 || _maxSpeed < _MIN_STEPPER_SPEED)) {
        stepTime = 1000000.0 / _maxSpeed;
        _dir = (_target > _current) ? 1 : -1;
    }
}

void Stepper::planner() {
    if (micros() - _plannerTime >= _plannerPrd) {
        _plannerTime = micros();
        long err = _target - _current;
        bool thisDir = ( _accelSpeed * _accelSpeed * _accelInv >= abs(err) );
        _accelSpeed += ( _accelTime * _plannerPrd * (thisDir ? -sign(_accelSpeed) : sign(err)));
        _accelSpeed = constrain(_accelSpeed, -_maxSpeed, _maxSpeed);

        if (abs(_accelSpeed) > _MIN_STEPPER_SPEED) stepTime = abs(1000000.0 / _accelSpeed);
        else stepTime = _MAX_STEP_PERIOD;
        _dir = sign(_accelSpeed);
    }
}

void    Stepper::reverse(bool dir)                  {  _dir = dir > 0 ? 1 : -1; }
void    Stepper::setCurrent(long pos) 		        { _current = pos; _accelSpeed = 0; }
void    Stepper::setCurrentDegrees(float pos) 	    { setCurrent((float)pos * _stepsPerDegrees); }
long    Stepper::getCurrent() 				        { return _current; }
float   Stepper::getCurrentDegrees() 			    { return ((float)_current / _stepsPerDegrees); }
void    Stepper::setTargetDegrees(float pos, 
            positioning type = ABSOLUTE)            { setTarget((float)pos * _stepsPerDegrees, type); }
long    Stepper::getTarget()                        { return _target; }
float   Stepper::getTargetDegrees()                 { return ((float)_target / _stepsPerDegrees); }
void    Stepper::setMaxSpeedDegrees(float speed)    { setMaxSpeed((float)speed * _stepsPerDegrees); }
void    Stepper::setAccelerationDegrees(float accel){ setAcceleration(accel * _stepsPerDegrees); }
void    Stepper::reset()                            { full_stop(); setCurrent(0); }
void    Stepper::setSpeedDegrees(float speed)       { setSpeed(_stepsPerDegrees * speed); }
float   Stepper::getSpeed()                         { return (1000000.0 / stepTime * _dir); }
float   Stepper::getSpeedDegrees()                  { return ((float)getSpeed() / _stepsPerDegrees); }
short   Stepper::getMicrosteps()                    { return _microstepping; }
int     Stepper::sign(int x)                        { return x >= 0 ? 1 : -1; }