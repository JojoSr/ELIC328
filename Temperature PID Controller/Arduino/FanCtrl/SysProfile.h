/*
 * SysProfile.h
 *
 * Created: 2018-11-24 10:35:27 AM
 *  Author: mjoha
 */ 


#ifndef SYSPROFILE_H_
#define SYSPROFILE_H_


#define FAN_TACH_1	3	// Fan Control Tach Line 1
#define FAN_TACH_2	2	// Fan Control Tach Line 2

#define LCD_ADDR 0x27

#define FAN_CTRL_1	6	// Fan PWM Control line 1
#define FAN_CTRL_2	5	// Fan PWM Control line 2

#define HEAT_CTRL1	10 // Heater Output 1 PWM
#define HEAT_CTRL2	11 // Heater Output 2 PWM

#define TEMP_IN_1	14	// Temperature input 1 reading
#define TEMP_IN_2	15	// Temperature input 2 reading

// set the deafult PID settings
#define DEFAULT_KI	0.01
#define DEFAULT_KP	0.01
#define DEFAULT_KD	0.01

#define PID_ERR_MAX 1.5		// maximum PID error sum
#define PID_ERR_MIN -1.5	// minimum PID error sum

#define DEFAULT_MIN_TEMP	15 // set the deafault minimum temperature
#define DEFAULT_MAX_TEMP	25 // set the deafult maximum temperature

#define ENC_PIN_A	2	// encoder A data pin
#define ENC_PIN_B	4	// encoder B data pin
#define ENC_PIN_BTM	3	// encder Button Pin

//#define VERBROS		1
#define DEBUG		1



#endif /* SYSPROFILE_H_ */