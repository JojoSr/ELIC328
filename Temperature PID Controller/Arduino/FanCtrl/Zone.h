// Zone.h

#ifndef _ZONE_h
#define _ZONE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include "SysProfile.h"

typedef struct{
	unsigned long fanRpm;
	uint8_t fanPwm;
} FAN_PROFILE;


typedef struct{
	double tempLast;
	double tempCurrent;
}TEMP_DATA;


typedef struct{
	double minTemp;
	double maxTemp;
	double setPoint;
	int samplingRate;
}TEMP_SETTINGS;


typedef struct{
	double ki;
	double kp;
	double kd;
	int sampleRate;
}PID_SETTINGS;


typedef struct{
	double lastError;
	double errSum;
	double output;
} PID_VARS;



class Zone
{
	protected:
	FAN_PROFILE fanProfile;
	TEMP_DATA tempData;
	TEMP_SETTINGS tempSettings;
	PID_SETTINGS pidSettings;
	PID_VARS pidVars;
	unsigned long lastTempSample, lastPidSample;
	uint8_t FanOutPin;
	uint8_t TempInPin;
	bool _isAuto;
	
	

	public:
	Zone();
	void init();
	void RunLoop();
	void ComputePid(double input);
	
	
	uint8_t GetFanOutputPin(){return FanOutPin;}
	void SetFanOutputPin(uint8_t val) { this->FanOutPin = val;}
	uint8_t GetTempInputPin(){return TempInPin; };
	void SetTempInputPin(uint8_t val) {this->TempInPin = val;}
	bool IsAuto(){return _isAuto; }		
	void IsAuto(bool aut){this->_isAuto = aut; }
	double GetPidOutput() { return this->pidVars.output; }	
	uint8_t GetFanPwmOutput(){return this->fanProfile.fanPwm; }		
	unsigned long GetFanRpm(){return this->fanProfile.fanRpm; }
	void SetFanRpm(unsigned long val){ this->fanProfile.fanRpm = val; }		
	double GetTemperature(){ return this->tempData.tempCurrent; }			
	double GetPidKi(){return this->pidSettings.ki; }		
	double GetPidKp(){return this->pidSettings.kp; }		
	double GetPidKd(){return this->pidSettings.kd; }						
	void SetPidKi(double val){ this->pidSettings.ki = val; }		
	void SetPidKp(double val){ this->pidSettings.kp = val; }		
	void SetPidKd(double val){ this->pidSettings.kd = val; }				
	int GetPidSamplingRate(){return this->pidSettings.sampleRate;}	
	void SetPidSamplingRate(int rate){this->pidSettings.sampleRate = rate; }		
	int GetTemperatureSamplingRate(){return this->tempSettings.samplingRate; }		
	void SetTemperatureSamplingRate(int rate){ this->tempSettings.samplingRate = rate; }
	double GetMinTemperatureSettings(){ return this->tempSettings.minTemp; }		
	double GetMaxTemperatureSettings(){ return this->tempSettings.maxTemp;}		
	double GetSetPointTemperature(){ return this->tempSettings.setPoint; }		
	void SetTemperatureRange(double min, double max){
		this->tempSettings.maxTemp = max;
		this->tempSettings.minTemp = min;
		this->tempSettings.setPoint = max-((max-min)/2);
	}
	double GetLastError(){ return this->pidVars.lastError; } 
	void SetPwmRate(uint8_t rate){ this->fanProfile.fanPwm = rate; }
};


#endif

