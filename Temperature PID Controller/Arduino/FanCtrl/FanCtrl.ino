#include "LabViewComm.h"

/*
Name:       FanCtrl.ino
Created:	2018-11-24 10:26:24 AM
Author:     RIPPER-9000\mjoha
*/
#include "SysProfile.h"
#include "Zone.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>



//////////////////////////////////////////////////////////////////////////
// Defines Primary states for state machine
//////////////////////////////////////////////////////////////////////////
enum class AppState {
	Starting,	// application is starting
	Manual,		// application is in manual mode
	Running,	// application is runing in auto mode
	Error,		// application is in an error state
	Unknown		// Unknown - Default State
};

//////////////////////////////////////////////////////////////////////////
// Defines the states for the menu selection
//////////////////////////////////////////////////////////////////////////
enum class MenuState {
	RootMenu,		// Root Menu slected
	SetTempRanges,	// set the temp ranges
	SetHeater,		// set the heater values
	SetManual,		// manual mode
	Unknown			// Unknown Default state
};


volatile unsigned int encoder0Pos = 0;
volatile bool encoderMoveUp = false;
volatile bool _isMenuActive = false;
volatile uint8_t _menuTabPos = 0;
volatile bool printHeader = true;
//////////////////////////////////////////////////////////////////////////
// Local Memory
//////////////////////////////////////////////////////////////////////////
unsigned long lastLabViewUpdate;
const uint8_t encoderSampleRate = 100;
const uint16_t encoderButtonRate = 500;
const uint16_t lcdRefreshRate = 200;
unsigned long lastEncoderCheck, lastLcdCheck, lastEncoderBtmCheck;
LabViewComm* comm;
Zone* zoneOne;
Zone* zoneTwo;
AppState _currentState = AppState::Starting;
MenuState _currentMenuState = MenuState::RootMenu;

volatile uint8_t heaterOne;
volatile uint8_t heaterTwo;

LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


//////////////////////////////////////////////////////////////////////////
// Interrupt for Encoder Roatation
//////////////////////////////////////////////////////////////////////////
void doEncoder_Expanded() {

	unsigned long now = millis();
	int timeTemp = (now - lastEncoderCheck);
	if (timeTemp >= encoderSampleRate) {
		if (digitalRead(ENC_PIN_A) == HIGH) {   // found a low-to-high on channel A
			if (digitalRead(ENC_PIN_B) == LOW) {  // check channel B to see which way
			  // encoder is turning
				encoder0Pos = encoder0Pos - 1;         // CCW
				encoderMoveUp = false;
			}
			else {
				encoder0Pos = encoder0Pos + 1;         // CW
				encoderMoveUp = true;
			}
		}
		else                                        // found a high-to-low on channel A
		{
			if (digitalRead(ENC_PIN_B) == LOW) {   // check channel B to see which way
			  // encoder is turning
				encoder0Pos = encoder0Pos + 1;          // CW
				encoderMoveUp = true;
			}
			else {
				encoder0Pos = encoder0Pos - 1;          // CCW
				encoderMoveUp = false;
			}

		}
		// handle menu states
		if (_isMenuActive)
		{
			switch (_currentMenuState)
			{
			case MenuState::SetTempRanges:
				if (_menuTabPos == 0) {
					if (encoderMoveUp) {
						double val = zoneOne->GetMinTemperatureSettings();
						val += 0.5;
						zoneOne->SetTemperatureRange(val, zoneOne->GetMaxTemperatureSettings());
					}
					else {
						double val = zoneOne->GetMinTemperatureSettings();
						val -= 0.5;
						zoneOne->SetTemperatureRange(val, zoneOne->GetMaxTemperatureSettings());
					}
				}
				else if (_menuTabPos == 1) {
					if (encoderMoveUp) {
						double val = zoneOne->GetMaxTemperatureSettings();
						val += 0.5;
						zoneOne->SetTemperatureRange(zoneOne->GetMinTemperatureSettings(), val);
					}
					else {
						double val = zoneOne->GetMaxTemperatureSettings();
						val -= 0.5;
						zoneOne->SetTemperatureRange(zoneOne->GetMinTemperatureSettings(), val);
					}
				}
				else if (_menuTabPos == 2) {
					if (encoderMoveUp) {
						double val = zoneTwo->GetMinTemperatureSettings();
						val += 0.5;
						zoneTwo->SetTemperatureRange(val, zoneTwo->GetMaxTemperatureSettings());
					}
					else {
						double val = zoneTwo->GetMinTemperatureSettings();
						val -= 0.5;
						zoneTwo->SetTemperatureRange(val, zoneTwo->GetMaxTemperatureSettings());
					}
				}
				else if (_menuTabPos == 3) {
					if (encoderMoveUp) {
						double val = zoneTwo->GetMaxTemperatureSettings();
						val += 0.5;
						zoneTwo->SetTemperatureRange(zoneTwo->GetMinTemperatureSettings(), val);
					}
					else {
						double val = zoneTwo->GetMaxTemperatureSettings();
						val -= 0.5;
						zoneTwo->SetTemperatureRange(zoneTwo->GetMinTemperatureSettings(), val);
					}
				}
				break;
			case MenuState::SetHeater:
				if (_menuTabPos == 0) {
					if (encoderMoveUp) {
						heaterOne = heaterOne + 5;
						if (heaterOne > 255)
							heaterOne = 255;
					}
					else {
						heaterOne = heaterOne - 5;
						if (heaterOne < 0)
							heaterOne = 0;
					}
				}
				else if (_menuTabPos == 1) {
					if (encoderMoveUp) {
						heaterTwo = heaterTwo + 5;
						if (heaterTwo > 255)
							heaterTwo = 255;
					}
					else {
						heaterTwo = heaterTwo - 5;
						if (heaterTwo < 0)
							heaterTwo = 0;
					}
				}

				break;
			case MenuState::SetManual:
				/*if (_menuTabPos == 0) {
					uint8_t pwm = zoneOne->GetFanPwmOutput();
					if (encoderMoveUp) {
						pwm = pwm + 5;
					}
					else {
						pwm = pwm - 5;
					}
					zoneOne->IsAuto(false);
					zoneOne->SetFanOutputPin(pwm);
				}
				else if (_menuTabPos == 1) {
					uint8_t pwm = zoneTwo->GetFanPwmOutput();
					if (encoderMoveUp) {
						pwm = pwm + 5;
					}
					else {
						pwm = pwm - 5;
					}
					zoneTwo->IsAuto(false);
					zoneTwo->SetFanOutputPin(pwm);
				}
				else if (_menuTabPos == 2) {
					encoder0Pos = 0;
					_menuTabPos = 0;
					zoneTwo->IsAuto(true);
					zoneTwo->IsAuto(true);
					_isMenuActive = false;
					_currentMenuState = MenuState::RootMenu;
				}*/
				break;
			case MenuState::Unknown:
			default:
				// do nothing
				break;
			}
		}


		lastEncoderCheck = now;
	}
}

//////////////////////////////////////////////////////////////////////////
// Interrupt for Encoder Button
//////////////////////////////////////////////////////////////////////////
void doEncoder_Button() {

	// determine if the button is a bounce
	unsigned long now = millis();
	int timeTemp = (now - lastEncoderBtmCheck);
	// if the refresh time has passed set flag to ture
	if (timeTemp >= encoderButtonRate) {
		if (_isMenuActive) {
			switch (_currentMenuState)
			{
			case MenuState::RootMenu:
				if (encoder0Pos == 0) {
					encoder0Pos = 0;
					_menuTabPos = 0;
					_currentMenuState = MenuState::SetTempRanges;
					printHeader = true;
				}
				else if (encoder0Pos == 1) {
					encoder0Pos = 0;
					_menuTabPos = 0;
					zoneOne->IsAuto(false);
					zoneTwo->IsAuto(false);
					_currentMenuState = MenuState::SetManual;
					_currentState = AppState::Manual;
					printHeader = true;
				}
				else if (encoder0Pos == 2) {
					encoder0Pos = 0;
					_menuTabPos = 0;
					_currentMenuState = MenuState::SetHeater;
					printHeader = true;
				}
				break;
			case MenuState::SetTempRanges:
				_menuTabPos++;
				if (_menuTabPos > 4)
					_menuTabPos = 0;
				break;
			case MenuState::SetHeater:
				if (_menuTabPos == 2) {
					_currentMenuState = MenuState::RootMenu;
					_isMenuActive = false;
					encoder0Pos = 0;
					_menuTabPos = 0;
					printHeader = true;
				}
				else {
					_menuTabPos++;
					if (_menuTabPos > 2)
						_menuTabPos = 0;
				}
				break;
			case MenuState::SetManual:
				if (_menuTabPos == 2) {
					_currentMenuState = MenuState::RootMenu;
					_isMenuActive = false;
					encoder0Pos = 0;
					_menuTabPos = 0;
					printHeader = true;
				}
				else {
					_menuTabPos++;
					if (_menuTabPos > 2)
						_menuTabPos = 0;
				}
				break;
			case MenuState::Unknown:
				break;
			default:
				break;
			}
		}
		else {
			_isMenuActive = true;
			printHeader = true;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Set up Devices
//////////////////////////////////////////////////////////////////////////
void setup()
{
	// LCD
	lcd.init();
	lcd.backlight();
	lcd.setCursor(0, 0);
	lcd.print(F("LabView Temp Control"));
	lcd.setCursor(0, 1);
	lcd.print(F("Starting Up ..."));
	lcd.setCursor(0, 2);
	lcd.print(F("Michael Johansen"));
	lcd.setCursor(0, 3);
	lcd.print(F("Edmond Malek"));
	delay(2000);



	// set up the pins
	pinMode(FAN_TACH_1, INPUT);
	pinMode(FAN_TACH_2, INPUT);
	pinMode(FAN_CTRL_1, OUTPUT);
	pinMode(FAN_CTRL_2, OUTPUT);
	pinMode(HEAT_CTRL1, OUTPUT);
	pinMode(HEAT_CTRL2, OUTPUT);
	pinMode(TEMP_IN_1, INPUT);
	pinMode(TEMP_IN_2, INPUT);

	pinMode(ENC_PIN_A, INPUT);
	digitalWrite(ENC_PIN_A, HIGH);       // turn on pull-up resistor
	pinMode(ENC_PIN_B, INPUT);
	digitalWrite(ENC_PIN_B, HIGH);       // turn on pull-up resistor
	pinMode(ENC_PIN_BTM, INPUT);
	digitalWrite(ENC_PIN_BTM, HIGH);

	attachInterrupt(digitalPinToInterrupt(ENC_PIN_A), doEncoder_Expanded, CHANGE);  // do encoder roatation interrupt
	attachInterrupt(digitalPinToInterrupt(ENC_PIN_BTM), doEncoder_Button, FALLING); // do encoder button interrupt

	// set the default heater values
	heaterOne = 0;
	heaterTwo = 0;

	// create the first zone to monitor
	zoneOne = new Zone();
	zoneOne->init();
	zoneOne->SetFanOutputPin(FAN_CTRL_1);
	zoneOne->SetTempInputPin(TEMP_IN_1);


	// cretae the second zone to monitor
	zoneTwo = new Zone();
	zoneTwo->init();
	zoneTwo->SetFanOutputPin(FAN_CTRL_2);
	zoneTwo->SetTempInputPin(TEMP_IN_2);

	// set up teh LabView Communication Layer
	comm = new LabViewComm(zoneOne, zoneTwo);

	// connect to the serial port / used for labview communication
	Serial.begin(115200);

	// set the deafult temp settings
	zoneOne->SetTemperatureRange(20, 30);
	zoneTwo->SetTemperatureRange(20, 30);
}

//////////////////////////////////////////////////////////////////////////
// Main Application Loop
//////////////////////////////////////////////////////////////////////////
void loop()
{
	// determine if the screen needs to be updated
	unsigned long now = millis();
	int timeTemp = (now - lastLcdCheck);
	bool updateScreen = false;
	// if the refresh time has passed set flag to ture
	if (timeTemp >= lcdRefreshRate) {
		updateScreen = true;
		lastLcdCheck = now;
	}

	// run zone loops
	zoneOne->RunLoop();
	zoneTwo->RunLoop();
	comm->RunLoop();

	// only refresh on screen update
	if (updateScreen) {
		
		// only display status if the menu is not active
		if (!_isMenuActive) {
			if (printHeader) {
				lcd.clear();
				// clear screen and write header
				lcd.setCursor(0, 0);
				lcd.print(F("LabView Temp Control"));
				// write zone one data to screen
				String fanText = "Temp:";
				fanText += zoneOne->GetTemperature();
				fanText += " Set:";
				fanText += zoneOne->GetSetPointTemperature();
				lcd.setCursor(0, 1);
				lcd.print(fanText);
				// write zone two to screen
				fanText = "Temp:";
				fanText += zoneTwo->GetTemperature();
				fanText += " Set:";
				fanText += zoneTwo->GetSetPointTemperature();
				lcd.setCursor(0, 2);
				lcd.print(fanText);
				printHeader = false;
			}
			else {
				lcd.setCursor(5, 1);
				lcd.print(zoneOne->GetTemperature());
				lcd.setCursor(15, 1);
				lcd.print(zoneOne->GetSetPointTemperature());

				lcd.setCursor(5, 2);
				lcd.print(zoneTwo->GetTemperature());
				lcd.setCursor(15, 2);
				lcd.print(zoneTwo->GetSetPointTemperature());
			}

			if (comm->HasUpdate()) {
				lcd.setCursor(0, 3);
				lcd.print(comm->GetLastUpdate());
				comm->ClearUpdate();
				delay(1000);
			}
		}
		else  // Display the active Menu Screen
		{
			switch (_currentMenuState)
			{
			case MenuState::RootMenu: // display the root menu
				if (printHeader) {
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print(F("Main Menu           "));
					lcd.setCursor(0, 1);
					lcd.print(F("  Set Temp Range    "));
					lcd.setCursor(0, 2);
					lcd.print(F("  Manual Control    "));
					lcd.setCursor(0, 3);
					lcd.print(F("  Set Heater        "));
					printHeader = false;
				}

				if (encoder0Pos > 2)
					encoder0Pos = 0;

				if (encoder0Pos == 0) {
					lcd.setCursor(0, 1);
					lcd.print("->");
					lcd.setCursor(0, 2);
					lcd.print("  ");
					lcd.setCursor(0, 3);
					lcd.print("  ");
				}
				else if (encoder0Pos == 1)
				{
					lcd.setCursor(0, 1);
					lcd.print("  ");
					lcd.setCursor(0, 2);
					lcd.print("->");
					lcd.setCursor(0, 3);
					lcd.print("  ");
				}
				else if (encoder0Pos == 2) {
					lcd.setCursor(0, 1);
					lcd.print("  ");
					lcd.setCursor(0, 2);
					lcd.print("  ");
					lcd.setCursor(0, 3);
					lcd.print("->");
				}

				break;
			case MenuState::SetTempRanges: // display set temp ranges menu
				lcd.setCursor(0, 0);
				lcd.print(F("Set Temp Ranges     "));

				if (_menuTabPos > 4)
					_menuTabPos = 0;

				if (_menuTabPos < 2) {
					lcd.setCursor(0, 1);
					lcd.print(F("Zone One:           "));
					lcd.setCursor(0, 2);
					lcd.print(F("   Min: "));
					lcd.print(zoneOne->GetMinTemperatureSettings());
					lcd.print("       ");
					lcd.setCursor(0, 3);
					lcd.print(F("   Max: "));
					lcd.print(zoneOne->GetMaxTemperatureSettings());
					lcd.print("       ");
				}
				else {
					lcd.setCursor(0, 1);
					lcd.print(F("Zone Two:"));
					lcd.setCursor(0, 2);
					lcd.print(F("   Min: "));
					lcd.print(zoneTwo->GetMinTemperatureSettings());
					lcd.print("       ");
					lcd.setCursor(0, 3);
					lcd.print(F("   Max: "));
					lcd.print(zoneTwo->GetMaxTemperatureSettings());
					lcd.print("       ");
				}

				if (_menuTabPos == 0 || _menuTabPos == 2) {
					lcd.setCursor(0, 2);
					lcd.print(F("*"));
				}
				else if (_menuTabPos == 4) {
					_currentMenuState = MenuState::RootMenu;
					_isMenuActive = false;
					encoder0Pos = 0;
				}
				else {
					lcd.setCursor(0, 3);
					lcd.print(F("*"));
				}
				break;
			case MenuState::SetHeater:	// display heater menu
				lcd.setCursor(0, 0);
				lcd.print(F("Set Heaters         "));

				double hOne = (heaterOne / 255.0) * 100;
				double hTwo = (heaterTwo / 255.0) * 100;

				Serial.println(hOne);

				lcd.setCursor(0, 1);
				lcd.print(F("  Heater One: "));
				lcd.print(hOne);
				lcd.print("   ");
				lcd.setCursor(0, 2);
				lcd.print(F("  Heater Two: "));
				lcd.print(hTwo);
				lcd.print("   ");
				lcd.setCursor(0, 3);
				lcd.print(F("  Finish            "));


				if (_menuTabPos == 0) {
					lcd.setCursor(0, 1);
					lcd.print("*");
					lcd.setCursor(0, 2);
					lcd.print("  ");
					lcd.setCursor(0, 3);
					lcd.print("  ");
				}
				else if (_menuTabPos == 1) {
					lcd.setCursor(0, 1);
					lcd.print("  ");
					lcd.setCursor(0, 2);
					lcd.print("*");
					lcd.setCursor(0, 3);
					lcd.print("  ");
				}
				else if (_menuTabPos == 2) {
					lcd.setCursor(0, 1);
					lcd.print("  ");
					lcd.setCursor(0, 2);
					lcd.print("  ");
					lcd.setCursor(0, 3);
					lcd.print("*");
				}

				break;
			case MenuState::SetManual:	// display manual menu
				Serial.println("Set Manual State");
				lcd.setCursor(0, 0);
				lcd.print(F("Set Manual     "));
				//double f1 = (zoneOne->GetFanPwmOutput() / 255.0) * 100;
				//double f2 = (zoneTwo->GetFanPwmOutput() / 255.0) * 100;


				/*lcd.setCursor(0, 1);
				lcd.print(F("  Zone 1: "));
				lcd.print(f1);
				lcd.print(F("%"));
				lcd.setCursor(0, 2);
				lcd.print(F("  Zone 2: "));
				lcd.print(f2);
				lcd.print(F("%"));
				lcd.setCursor(0, 3);
				lcd.print(F("  Finished"));


				if (_menuTabPos > 2)
					_menuTabPos = 0;

				if (_menuTabPos == 0) {
					lcd.setCursor(0, 1);
					lcd.print("*");
					lcd.setCursor(0, 2);
					lcd.print("  ");
					lcd.setCursor(0, 3);
					lcd.print("  ");
				}
				else if (_menuTabPos == 1) {
					lcd.setCursor(0, 1);
					lcd.print("  ");
					lcd.setCursor(0, 2);
					lcd.print("*");
					lcd.setCursor(0, 3);
					lcd.print("  ");
				}
				else {
					lcd.setCursor(0, 1);
					lcd.print("  ");
					lcd.setCursor(0, 2);
					lcd.print("  ");
					lcd.setCursor(0, 3);
					lcd.print("*");
				}*/

				break;

			case MenuState::Unknown: // unknown menu - reset to root menu
				_currentMenuState = MenuState::RootMenu;
				break;
			default:
				_currentMenuState = MenuState::RootMenu;
				break;
			}
		}
	}
}
