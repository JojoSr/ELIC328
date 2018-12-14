// 
// 
// 

#include "LabViewComm.h"


LabViewComm::LabViewComm(Zone * zone, Zone* ztwo)
{
	this->_zoneOne = zone;
	this->_zoneTwo = ztwo;
	labSettings.SampleRate = 500;
}

void LabViewComm::RunLoop()
{
	
	/*
	Update Data

	{
		"command":"update",
		"data":{
			"zones":[
			{
				"minTemp":20.0,
				"maxTemp":30.0,
				"tempSampleRate":1000,
				"ki":0.01,
				"kp":0.01,
				"kd":0.01,
				"pidSampleRate":1000
			},
			{
				"minTemp":20.0,
				"maxTemp":30.0,
				"tempSampleRate":1000,
				"ki":0.01,
				"kp":0.01,
				"kd":0.01,
				"pidSampleRate":1000
			}
		]}
	}
	
	
	Manual Data

	{
		"command":"manual",
		"data":{"numzones":2,
			"zones":[
			{
				"pwmValue":20.0
			},
			{
				"pwmValue":20.0
			}
		]}
	}
	
	*/
	
	
	// Check for command on serial port
	if(Serial.available() > 0)
	{
		String dataIns = Serial.readStringUntil(';');
		
		StaticJsonBuffer<575> jsonBuffer;
		
		JsonObject& root = jsonBuffer.parseObject(dataIns);
		const char* command = root["command"];
		this->_hasUpdate = true;
		lastUpdate = command;

		if(strcmp(command, "update") == 0){
			this->_hasUpdate = true;
			lastUpdate = dataIns;
			int numzones = 2;
			for(int i=0;i<numzones;i++){
				JsonObject& zone = root["data"]["zones"][i];
				
				double minTemp = zone["minTemp"]; // 20
				double maxTemp = zone["maxTemp"]; // 30
				int tempSampleRate = zone["tempSampleRate"]; // 1000
				double ki = zone["ki"]; // 0.01
				double kp = zone["kp"]; // 0.01
				double kd = zone["kd"]; // 0.01
				int pidSampleRate = zone["pidSampleRate"]; // 1000
				
				if(i == 0){
					this->_zoneOne->SetTemperatureRange(minTemp, maxTemp);
					this->_zoneOne->SetTemperatureSamplingRate(tempSampleRate);
					this->_zoneOne->SetPidKd(kd);
					this->_zoneOne->SetPidKi(ki);
					this->_zoneOne->SetPidKp(kp);
					this->_zoneOne->SetPidSamplingRate(pidSampleRate);
				}else if(i==1){
					this->_zoneTwo->SetTemperatureRange(minTemp, maxTemp);
					this->_zoneTwo->SetTemperatureSamplingRate(tempSampleRate);
					this->_zoneTwo->SetPidKd(kd);
					this->_zoneTwo->SetPidKi(ki);
					this->_zoneTwo->SetPidKp(kp);
					this->_zoneTwo->SetPidSamplingRate(pidSampleRate);
				}
			}		
			
		}else if(strcmp(command, "manual") == 0){
			int manual_numzones = 2; // 2
			
			for(int i=0;i<manual_numzones;i++){
				int manual_zones0_pwmValue = root["data"]["zones"][i]["pwmValue"]; // 20
				if(i==0){
					this->_zoneOne->IsAuto(false);
					this->_zoneOne->SetPwmRate(manual_zones0_pwmValue);
				}else if(i==1){
					this->_zoneTwo->IsAuto(false);
					this->_zoneTwo->SetPwmRate(manual_zones0_pwmValue);
				}
			}
			
		}else if(strcmp(command, "auto") == 0){
			this->_zoneOne->IsAuto(true);
			this->_zoneTwo->IsAuto(true);
		}
	}
	
	// check for lab view update
	unsigned long now = millis();
	int timLeft = now-lastLabViewUpdate;
	if(timLeft >= labSettings.SampleRate)
	{
		String json = "{\"action\":\"update\",\"data\":{\"zone\":[{\"temp\":";
		json += this->_zoneOne->GetTemperature();
		json += ",\"fanRpm\":";
		json += this->_zoneOne->GetFanRpm();
		json += ",\"fanPwm\":";
		json += this->_zoneOne->GetFanPwmOutput();
		json += ",\"error\":";
		json+= this->_zoneOne->GetLastError();
		json += ",\"pidout\":";
		json+= this->_zoneOne->GetPidOutput();
		json +="},{\"temp\":";
		json += this->_zoneTwo->GetTemperature();
		json += ",\"fanRpm\":";
		json += this->_zoneTwo->GetFanRpm();
		json += ",\"fanPwm\":";
		json += this->_zoneTwo->GetFanPwmOutput();
		json += ",\"error\":";
		json+= this->_zoneTwo->GetLastError();
		json += ",\"pidout\":";
		json+= this->_zoneTwo->GetPidOutput();
		json +="}";		
		json +="]}};";
		
		Serial.println(json);
		Serial.flush();
		lastLabViewUpdate = now;
	}
}


