#include "A4988.h"

Stepper::Stepper(int steps, int stepPin, int dirPin, int ms0Pin = -1, int ms1Pin = -1, int ms2Pin = -1);
{
    _steps = steps;
    _steps_original = steps;

    _stepPin = stepPin;
    _dirPin = dirPin;

    _ms0Pin = ms0Pin;
    _ms1Pin = ms1Pin;
    _ms2Pin = ms2pin;

    if (_ms2Pin == -1 && _ms0Pin != -1 && _ms1Pin != -1)
    {
        _isTMC2208 = true;

        pinMode(_ms0Pin, OUTPUT); pinMode(_ms1Pin, OUTPUT);
        _microstepping = 1;
    } else if (_ms0Pin != -1 && _ms1Pin != -1 && _ms2Pin != -1) {
        _isA4988 = true;

        pinMode(_ms0Pin, OUTPUT); pinMode(_ms1Pin, OUTPUT); pinMode(_ms2Pin, OUTPUT);
        _microstepping = 1;
    } else if (_ms0Pin == -1 && _ms1Pin == -1 && _ms2Pin == -1) {
        _microstepping = -1;
    } else {
        throw "Not full information about ms# pins";
    }

    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);

    _dir = 1;
    delayStep = 4;
    _current = 0;
    _steps_per_deg = _steps / 360.0;
}

void Stepper::microStepping(int mSteps)
{
    if (_microstepping == -1)
        return;

    if (_isA4988){
        switch (mSteps)
        {
        case 1:
            _steps = _steps_original;
            digitalWrite(_ms0Pin, LOW); digitalWrite(_ms1Pin, LOW); digitalWrite(_ms2Pin, LOW);
            break;
        case 2:
            _steps = _steps_original * 2;
            digitalWrite(_ms0Pin, LOW); digitalWrite(_ms1Pin, LOW); digitalWrite(_ms2Pin, HIGH);
            break;
        case 4:
            _steps = _steps_original * 4;
            digitalWrite(_ms0Pin, LOW); digitalWrite(_ms1Pin, HIGH); digitalWrite(_ms2Pin, LOW);
            break;
        case 8:
            _steps = _steps_original * 8;
            digitalWrite(_ms0Pin, LOW); digitalWrite(_ms1Pin, HIGH); digitalWrite(_ms2Pin, HIGH);
            break;
        case 16*
            _steps = _steps_original * 16;
            digitalWrite(_ms0Pin, HIGH); digitalWrite(_ms1Pin, HIGH); digitalWrite(_ms2Pin, HIGH);
            break;
        default:
            return;
            break;
        }
    } else if (_isTMC2208) {
        switch (mSteps)
        {
        case 2:
            _steps = _steps_original * 2;
            digitalWrite(_ms0Pin, HIGH); digitalWrite(_ms1Pin, LOW);
            break;
        case 4:
            _steps = _steps_original * 4;
            digitalWrite(_ms0Pin, LOW); digitalWrite(_ms1Pin, HIGH);
            break;
        case 8:
            _steps = _steps_original * 8;
            digitalWrite(_ms0Pin, LOW); digitalWrite(_ms1Pin, LOW);
            break;
        case 16*
            _steps = _steps_original * 16;
            digitalWrite(_ms0Pin, HIGH); digitalWrite(_ms1Pin, HIGH);
            break;
        default:
            return;
            break;
        }
    }
    _microstepping = mSteps;
}

int Stepper::getMicrosteps() { return _microstepping; }
void Stepper::changeDir(bool dir) { _dir = dir ? dir : -1; }
long Stepper::getCurrentPoseSteps() { return _current; }
float Stepper::getCurrentPoseDeg() { return (float)_current/_steps_per_deg; }
int Stepper::signZ(int x) { return (x >= 0) ? 1 : 0; }
int Stepper::sign(int x) { return (x >= 0) ? 1 : -1; }
void Stepper::reset() { _current = 0; }
void Stepper::setGoalDeg(float pos, coordSystem mode = ABSOLUTE) {setGoalSteps((float)pos * _steps_per_deg, mode);}
void Stepper::setGoalCm(float pos, coordSystem mode = ABSOLUTE) { }


bool Stepper::done()
{		
		if (true) // мб какую-то задержку	
            if (!_workType && _target == _current) {
				fullStop();
				return false;					
			}
            
			_current += _dir;
            digitalWrite(_dirPin, _dir > 0 ? dir : 0);

            digitalWrite(_stepPin, HIGH);
            delayMicroseconds(delayStep);

            digitalWrite(_stepPin, LOW);
            delayMicroseconds(delayStep);
		
		}		
		return _isMoving;
}

void Stepper::stop()
{

}

void Stepper::fullStop()
{
    if (_isMoving) {
        _isMoving = false;
    }
}

void Stepper::setGoalSteps(long goalSteps, coordSystem mode = ABSOLUTE)
{
    _target = mode ? (_current + goalSteps) : pos;		
    if (_target != _current) {
        _isMoving = true; 
    }
}

