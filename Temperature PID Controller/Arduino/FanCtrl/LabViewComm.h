// LabViewComm.h

#ifndef _LABVIEWCOMM_h
#define _LABVIEWCOMM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////
// Provides the settings for lab view
//////////////////////////////////////////////////////////////////////////
typedef struct{
	int SampleRate;
}LabView_Settings;

class LabViewComm
{
 protected:
	Zone* _zoneOne;
	Zone* _zoneTwo;
	unsigned long lastLabViewUpdate;
	LabView_Settings labSettings;
	bool _hasUpdate = false;
	String lastUpdate = "";
 public:
	LabViewComm(Zone * zone, Zone* ztwo);
	void RunLoop();
	bool HasUpdate() { return _hasUpdate;  }
	void ClearUpdate() { _hasUpdate = false; }

	String GetLastUpdate() { return lastUpdate;  }
};


#endif

