// 
// 
// 

#include "Zone.h"

 Zone::Zone()
{
	this->fanProfile.fanPwm = 0;
	this->fanProfile.fanRpm = 0;
	
	this->pidSettings.ki = 0.1;
	this->pidSettings.kp = 0.2;
	this->pidSettings.kd = 0.1;
	this->pidSettings.sampleRate = 100;
	
	this->pidVars.lastError = -9999;
	this->pidVars.errSum = 0;
	this->pidVars.output = 0;
	
	this->tempData.tempCurrent = 0;
	this->tempData.tempLast = 0;
	
	this->tempSettings.minTemp = 15;
	this->tempSettings.maxTemp = 30;
	this->tempSettings.setPoint = this->tempSettings.maxTemp-((this->tempSettings.maxTemp-this->tempSettings.minTemp)/2);
	this->tempSettings.samplingRate = 100;
	
	this->lastPidSample = millis()-this->pidSettings.sampleRate;
	this->lastTempSample = millis()-this->tempSettings.samplingRate;	
}

void Zone::init()
{
	this->tempSettings.setPoint = this->tempSettings.maxTemp-((this->tempSettings.maxTemp-this->tempSettings.minTemp)/2);
	this->lastPidSample = millis()-this->pidSettings.sampleRate;
	this->lastTempSample = millis()-this->tempSettings.samplingRate;
}

void Zone::RunLoop()
{
	unsigned long now = millis();
	int timeTemp = (now - lastTempSample);
	int val1;
	
	// if the current temp sample is out of date
	if(timeTemp >=this->tempSettings.samplingRate){
		tempData.tempLast = tempData.tempCurrent;
		val1 = analogRead(TempInPin);
		double mv = (val1/1024.0)*5000;
		double cel = mv/10;
		tempData.tempCurrent = cel;
	}
	
	if(this->IsAuto()){
		// compute PID
		ComputePid(tempData.tempCurrent);
	
		if(pidVars.output <= 0){
			int pwm1 = (255 * (pidVars.output * -1));
			if(pwm1 > 255)
				this->fanProfile.fanPwm = 255;
			else if(pwm1 < 0)
				this->fanProfile.fanPwm = 0;
			else
				this->fanProfile.fanPwm = pwm1;	
		}else{
			this->fanProfile.fanPwm = 0;
		}
	}
	analogWrite(FanOutPin, this->fanProfile.fanPwm);
}

void Zone::ComputePid(double input)
{
	unsigned long now = millis();
	int timeChnage = (now-this->lastPidSample);
	// if the sample is out of date
	if(timeChnage>=this->pidSettings.sampleRate)
	{
		// TODO : verify limits
		// place a limit on the error sum
		if(pidVars.errSum < PID_ERR_MIN){
			pidVars.errSum = PID_ERR_MIN; // will drive the fan at 150%
		}else if (pidVars.errSum > PID_ERR_MAX){
			pidVars.errSum =  PID_ERR_MAX; // limit in pace to prevent errSum from going to far out of spec
		}
		
		
		// compute PID
		double error = tempSettings.setPoint-input; // current error value
		pidVars.errSum += error; // sum error
		double dErr = (error - pidVars.lastError); // error dirivtive
		
		pidVars.output = pidSettings.kp*error+pidSettings.ki*pidVars.errSum+pidSettings.kd*dErr; // compute output
		pidVars.lastError = error; // update error value
		lastPidSample = now; // update sampletime
	}
}
