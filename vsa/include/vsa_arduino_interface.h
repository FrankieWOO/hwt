/** 
 * \file vsa_arduino_interface.h
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 *
 * \brief Module for interfacing to variable stiffness actuators (e.g., Edinburgh VSA, MACCEPA).
 */
#ifndef __vsa_arduino_interface_h
#define __vsa_arduino_interface_h

#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#else
#include <pthread.h>
#endif

#ifdef MACCEPA_INTERFACE
/* MACCEPA-specific defines */
#include "../sketchbook/maccepa/defines.h"
/** \brief Macro for converting commands to Servo 0 in radiens (with a range of  -pi/2 < u < pi/2) to units of 0.5 microseconds. */
#define RAD2USEC_SERVO0(u) U_MIN_SERVO0+(int)((U_MAX_SERVO0-U_MIN_SERVO0)*((-u+(M_PI/2))/M_PI)) /* Equilibrium position motor in range -pi/2 < u1 < pi/2 */
/** \brief Macro for converting commands to Servo 1 in radiens (with a range of  0 < u < pi/2) to units of 0.5 microseconds. */

#define RAD2USEC_SERVO1(u) U_MIN_SERVO1+(int)((U_MAX_SERVO1-U_MIN_SERVO1)*(u-U_MIN_RAD_SERVO1)/(U_MAX_RAD_SERVO1-U_MIN_RAD_SERVO1)) 
/** Alex: change calculation of command depending on orientation of drum winding. Will default to clockwise
 * unless DRUM_WINDING_COUNTERCLOCKWISE is defined */
#ifdef DRUM_WINDING_COUNTERCLOCKWISE
#define RAD2USEC_SERVO1(u) U_MIN_SERVO1+(int)((U_MAX_SERVO1-U_MIN_SERVO1)*(-u+U_MAX_RAD_SERVO1)/(U_MAX_RAD_SERVO1-U_MIN_RAD_SERVO1)) 
#endif

/** \brief Macro for converting commands to Servo 1 in radiens (with a range of  -pi/2 < u < pi/2) to units of 0.5 microseconds. */
#define DUTY2PWM_DAMPER0(u) U_MIN_DAMPER0+(int)((U_MAX_DAMPER0-U_MIN_DAMPER0)*(u-U_MIN_DUTY_DAMPER0)/(U_MAX_DUTY_DAMPER0-U_MIN_DUTY_DAMPER0)) 
/* \brief Dimension of observation vector (number of sensors). */
/* #define DIMY 5 
 */
/* \brief Dimension of control vector (number of servos). */
/* #define DIMU 2
 */
#endif

#ifdef MACCEPA2DOF_INTERFACE
/* MACCEPA2DOF-specific defines */
#include "../sketchbook/maccepa2dof/defines.h"
/** \brief Macro for converting commands to Servo 0 in radiens (with a range of  -pi/2 < u < pi/2) to units of 0.5 microseconds. */
#define RAD2USEC_SERVO0(u) U_MIN_SERVO0+(int)((U_MAX_SERVO0-U_MIN_SERVO0)*((-u+(M_PI/2))/M_PI)) 
/** \brief Macro for converting commands to Servo 1 in radiens (with a range of  0 < u < pi) to units of 0.5 microseconds. */
#define RAD2USEC_SERVO1(u) U_MIN_SERVO1+(int)((U_MAX_SERVO1-U_MIN_SERVO1)*(u-U_MIN_RAD_SERVO1)/(U_MAX_RAD_SERVO1-U_MIN_RAD_SERVO1)) 
/** \brief Macro for converting commands to Servo 2 in radiens (with a range of  -pi/2 < u < pi/2) to units of 0.5 microseconds. */
#define RAD2USEC_SERVO2(u) U_MIN_SERVO2+(int)((U_MAX_SERVO2-U_MIN_SERVO2)*((u+(M_PI/2))/M_PI)) 
/** \brief Macro for converting commands to Servo 3 in radiens (with a range of  0 < u < pi) to units of 0.5 microseconds. */
#define RAD2USEC_SERVO3(u) U_MIN_SERVO3+(int)((U_MAX_SERVO3-U_MIN_SERVO3)*(u-U_MIN_RAD_SERVO3)/(U_MAX_RAD_SERVO3-U_MIN_RAD_SERVO3)) 
#endif

#ifdef EDINBURGHVSA_INTERFACE
/* Edinburgh VSA-specific defines */
#include "../sketchbook/edinburghvsa/defines.h"
#define RAD2USEC_SERVO0(u) U_MIN_SERVO0+(int)((U_MAX_SERVO0-U_MIN_SERVO0)*((M_PI-u)/M_PI)) 
#define RAD2USEC_SERVO1(u) U_MIN_SERVO1+(int)((U_MAX_SERVO1-U_MIN_SERVO1)*(u/M_PI)) 
/* \brief Dimension of observation vector (number of sensors + 1 element for time stamps). */
/* #define DIMY 4 
 */
/* \brief Dimension of control vector (number of servos). */
/* #define DIMU 2
 */
#endif

#define ARDUINO_DISCONNECTED 0
#define ARDUINO_CONNECTED 1
#define THREAD_KILLED 1
/* \brief Timeout in milliseconds when waiting for serial communication from Arduino. */
#define TIMEOUT 100

/* Define locking of shared data for windows/linux */
#ifdef WIN32
#define LOCK(AI)      EnterCriticalSection(&((AI)->threadLock))
#define UNLOCK(AI)    LeaveCriticalSection(&((AI)->threadLock))
#else
#include <unistd.h>
#include <time.h>
#define LOCK(AI)      pthread_mutex_lock(&((AI)->threadLock))
#define UNLOCK(AI)    pthread_mutex_unlock(&((AI)->threadLock))
#endif   

#include <serial.h>

/** \brief Arduino Interface for handling communications between user programs and the arduino control boards of
 * variable stiffness actuators.
*/
typedef struct {
	/** \brief Serial port struct. */
	SerialPort port;
	/* \brief Flag for determining when reading a package from the serial port is complete. */
	int readComplete;
	/** \brief Arduino connection state. connection_state=ARDUINO_DISCONNECTED, 
	 * if no connection, connection_state=ARDUINO_CONNECTED if connection has 
	 * been made (connection_state=2 kills the thread)  */
	int connection_state;

	/** \brief Buffer for sending motor commands. The first element of this is
	 * the 'header' char RECEIVE_HEADER, the remaining elements contain motor
	 * commands. 
	 */
	unsigned char command_buffer[RECEIVE_LENGTH+1];	
	/** \brief Array containing measured state (i.e., readings from the Arduino ADCs). */
	int measured_state[DIMY];
	/** \brief Timestamp at which measured state is read (i.e., when ADC readings are received from Arduino). */
	double timestamp;

#ifdef WIN32
	CRITICAL_SECTION threadLock;
	HANDLE thread;
#else
	/** \brief Mutex (for locking shared data structures.) */
	pthread_mutex_t threadLock;
	/** \brief I/O thread */
	pthread_t thread;
#endif
	/** \brief I/O thread state. Thread runs while thread_state!=THREAD_KILLED. */
	int thread_state;

} ArduinoInterface;


/** 
 * \brief Initialise serial connection with Arduino board, start i/o thread in the background. 
 * \param[in] device Name of serial port to which Arduino is connected.
 * \param AI arduino interface.
 */
int  vsa_arduino_interface_init       ( ArduinoInterface *AI, char *device );
/** 
 * \brief Close serial connection with Arduino board, shutdown background i/o thread. 
 * \param AI arduino interface.
 */
void vsa_arduino_interface_close      ( ArduinoInterface *AI );
/** 
 * \brief Check that Arduino connection has been set up.
 * \param AI arduino interface.
 */
int vsa_arduino_interface_check       ( ArduinoInterface *AI );
/** 
 * \brief Command new motor positions u. 
 *
 * \param AI arduino interface.
 * \param[in] u[DIMU] array of desired motor positions (rad).
 *
 * Commands are clipped to within U_LLIM_RAD_SERVOx < u[x] < U_ULIM_RAD_SERVOx,
 * and converted to units of .5 usec, before being sent to the Arduino.
 *
 * \note This function will NOT block, and there is no guarantee about when the
 * new positions will be sent to the servos.
 *
 */
void vsa_arduino_interface_write      ( ArduinoInterface *AI, double * u );
/** 
 * \brief Command new motor positions u. 
 *
 * \param AI arduino interface.
 * \param[in] u[DIMU] array of desired motor positions (.5 usec).
 *
 * Commands are sent directly to the Arduino. This command mode is meant only
 * for configuring the servos. NO SAFETY LIMITS ARE APPLIED!
 *
 * \note This function will NOT block, and there is no guarantee about when the
 * new positions will be sent to the servos.
 */
void vsa_arduino_interface_write_usec ( ArduinoInterface *AI, int * u );
/** 
 * \brief Read sensors. Values returned will be converted into appropriate units (e.g., angles in radiens, power in W, etc.). 
 * \param AI arduino interface.
 * \param[out] y double array, containing sensor readings.
 *
 * \note Calling this function will block until the next frame of data has arrived from the Arduino.
 */
void vsa_arduino_interface_read       ( ArduinoInterface *AI, double *y );
/** 
 * \brief Read sensors. Values returned will be as steps of the ADCs. 
 * \param AI arduino interface.
 * \param[out] y double array, containing sensor readings.
 *
 * \note Calling this function will block until the next frame of data has arrived from the Arduino.
 */
void vsa_arduino_interface_read_adc ( ArduinoInterface *AI, int *y );
/** 
 * \brief Read sensors from the arm, and command new motor positions. 
 * \param AI arduino interface.
 * \param[in] u desired motor positions (rad).
 * \param[out] y sensor readings (same units as read).
 *
 */
void vsa_arduino_interface_run_step ( ArduinoInterface *AI, double *u, double *y );

#endif
