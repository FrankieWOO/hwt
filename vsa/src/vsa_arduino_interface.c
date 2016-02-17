/** 
 * \file vsa_arduino_interface.c
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 *
 * \brief Module for interfacing to variable stiffness actuators (e.g., Edinburgh VSA, MACCEPA).
 */
#include <vsa_arduino_interface.h>
#ifdef MEX_INTERFACE
#include <mex.h>
#endif

/** 
 * \brief I/O thread function. Continually runs in the background, 
 * reading from the serial. If new data arrives at the serial it is decoded and written to the measured_state variable.
 */
#ifdef WIN32
static void threadFunc(LPVOID data) {
   DWORD timeZero;
#else
static void *threadFunc(void *data) {
	struct timespec tSpec, tSpec0;
#endif   
   
	ArduinoInterface *AI = (ArduinoInterface *) data;

	int i;

	int input = 0;      /** \brief Variable for temporary storage of incoming serial data. */
	int read_state = 0; /** \brief Variable for keeping track of the stage of parsing of incoming serial data. */
	int read_sensor = 0;/** \brief Variable for keeping track of which sensor we are reading from. */
	int y[DIMY];        /** \brief Variable for temporary storage of sensor readings during parsing. */
	double timestamp;   /** \brief Time stamp of data. */

#ifdef WIN32
	timeBeginPeriod(1);
	timeZero = timeGetTime();
#else
	clock_gettime(CLOCK_REALTIME, &tSpec0);
#endif

	AI->thread_state=!THREAD_KILLED;
	while (AI->thread_state != THREAD_KILLED ) { /* Loop until thread is killed. */
		if (serialRead(&AI->port, 1, &input) == 0) continue;

		/* Process bit string from serial */
		switch(read_state) {
			case  0: if (input == TRANSMIT_HEADER0) read_state++;                break; /* Read first header. */
			case  1: if (input == TRANSMIT_HEADER1) read_state++;                break; /* Read second header. */
			case  2: y[read_sensor]  = input    ;   read_state++;                break;
			case  3: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case  4: y[read_sensor]  = input    ;   read_state++;                break;
			case  5: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case  6: y[read_sensor]  = input    ;   read_state++;                break;
			case  7: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case  8: y[read_sensor]  = input    ;   read_state++;                break;
#ifdef EDINBURGHVSA_INTERFACE
			case  9: y[read_sensor] += input*256;   read_state=0; read_sensor=0; /* Reset read_state. */
#endif
#ifdef MACCEPA_INTERFACE
			case  9: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case 10: y[read_sensor]  = input    ;   read_state++;                break;
			case 11: y[read_sensor] += input*256;   read_state=0; read_sensor=0; /* Reset read_state. */
#endif
#ifdef MACCEPA2DOF_INTERFACE
			case  9: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case 10: y[read_sensor]  = input    ;   read_state++;                break;
			case 11: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case 12: y[read_sensor]  = input    ;   read_state++;                break;
#ifdef CURRENT_SENSING
			case 13: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case 14: y[read_sensor]  = input    ;   read_state++;                break;
			case 15: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case 16: y[read_sensor]  = input    ;   read_state++;                break;
			case 17: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case 18: y[read_sensor]  = input    ;   read_state++;                break;
			case 19: y[read_sensor] += input*256;   read_state++; read_sensor++; break;
			case 20: y[read_sensor]  = input    ;   read_state++;                break;
			case 21: y[read_sensor] += input*256;   read_state=0; read_sensor=0; /* Reset read_state. */
#else
			case 13: y[read_sensor] += input*256;   read_state=0; read_sensor=0; /* Reset read_state. */
#endif
#endif

#ifdef WIN32
				timestamp = (timeGetTime() - timeZero) * 0.001;
#else
				clock_gettime(CLOCK_REALTIME, &tSpec);
				timestamp = (tSpec.tv_sec - tSpec0.tv_sec) + 1e-9*tSpec.tv_nsec;
#endif
				LOCK(AI);
				for ( i = 0; i < DIMY; i += 1 ) { AI->measured_state[i] = y[i]; }
				AI->timestamp = timestamp;
				serialWrite(&AI->port, RECEIVE_LENGTH+1, AI->command_buffer); /* Write command to serial */
				serialFlushOutput (&AI->port); /* flush i/o */
				serialFlushInput  (&AI->port);
				AI->readComplete = 1;
				UNLOCK(AI);
		}
	}

#ifdef WIN32
	timeEndPeriod(1);
	return;
#else
	return NULL;
#endif
}

/** 
 * \brief Open serial connection with Arduino board, start i/o thread in the background. 
 * \param[in] device Name of serial port to which Arduino is connected.
*/
int  vsa_arduino_interface_init       ( ArduinoInterface *AI, char *device ) {
	/* Check if port is already open. */
#ifdef MEX_INTERFACE
	if (AI->connection_state == ARDUINO_CONNECTED) {
		mexWarnMsgTxt("Arduino is already open!");
		return 1;
	}
#endif
	/* Open serial communication. */
	AI->connection_state = serialOpenByName(&AI->port, device);
	if (AI->connection_state==ARDUINO_DISCONNECTED) {
#ifdef MEX_INTERFACE
		mexErrMsgTxt("Couldn't open the specified port.");
#else
		fputs("Couldn't open the specified port.\n",stderr);
		return 0;
#endif
	} 
	/* Set serial parameters (or close the serial if this fails). */
	if (!serialSetParameters(&AI->port,BAUDRATE, 8, 0, 1, 10)) { /* BAUDRATE baud, 8 data bits, no parity, 1 stop bit, 10*0.1 = 1 sec. timeout */
		AI->connection_state = ARDUINO_DISCONNECTED;
		serialClose(&AI->port);
#ifdef MEX_INTERFACE
		mexErrMsgTxt("Couldn't change serial port parameters!");
#else
		fputs("Couldn't change serial port parameters!\n",stderr);
		return 0;
#endif
	}

	/* Set initial command */
	AI->command_buffer[ 0] = RECEIVE_HEADER;        /* header */
	AI->command_buffer[ 1] = U_INIT_SERVO0 & 0xFE;
	AI->command_buffer[ 2] = U_INIT_SERVO0 >> 8;
	AI->command_buffer[ 3] = U_INIT_SERVO1 & 0xFE;
	AI->command_buffer[ 4] = U_INIT_SERVO1 >> 8;
#ifdef MACCEPA2DOF_INTERFACE
	AI->command_buffer[ 5] = U_INIT_SERVO2 & 0xFE;
	AI->command_buffer[ 6] = U_INIT_SERVO2 >> 8;
	AI->command_buffer[ 7] = U_INIT_SERVO3 & 0xFE;
	AI->command_buffer[ 8] = U_INIT_SERVO3 >> 8;
#ifdef VARIABLE_DAMPING
	AI->command_buffer[ 9] = U_INIT_DAMPER0;
	AI->command_buffer[10] = 0x00;
	AI->command_buffer[11] = U_INIT_DAMPER1;
	AI->command_buffer[12] = 0x00;
#ifdef MAGNET /* if damping and magnet control */
	AI->command_buffer[13] = U_INIT_MAGNET;
	AI->command_buffer[14] = 0x00;
#endif
#else
#ifdef MAGNET /* if just magnet control */
	AI->command_buffer[ 9] = U_INIT_MAGNET;
	AI->command_buffer[10] = 0x00;
#endif
#endif

#endif

	/* Flush serial port. */
	serialFlushInput(&AI->port);
	serialFlushOutput(&AI->port);

	/* Start i/o thread. */
#ifdef WIN32
   InitializeCriticalSection(&AI->threadLock);
   AI->thread = (HANDLE) _beginthread(threadFunc, 0, AI);
   if (AI->thread == NULL) {
      return 0;
   }
#else
   pthread_mutex_init(&AI->threadLock, NULL);
   pthread_create(&AI->thread, NULL, threadFunc, AI);
#endif
   
	/* Connection successful so set connection_state to ARDUINO_CONNECTED */
	AI->connection_state = ARDUINO_CONNECTED;

#ifdef MEX_INTERFACE
	/* Prevent matlab from clearing mex file */
	mexLock();
#endif

	return 1;
}

/** 
 * \brief Check that Arduino connection has been set up.
 */
int vsa_arduino_interface_check ( ArduinoInterface *AI ) {
	if (AI->connection_state == 0) {
#ifdef MEX_INTERFACE
		mexErrMsgTxt("Arduino connection has not been set up yet.");
#else
		fputs("Arduino connection has not been set up yet.",stderr);
		return 0;
#endif
	}
	return 1;
}

/** 
 * \brief Close serial connection with Arduino board, stop background i/o thread. 
 */
void vsa_arduino_interface_close      ( ArduinoInterface *AI ) {
	/* If already closed, return. */
	if (AI->connection_state == ARDUINO_DISCONNECTED) return;

	/* Kill the i/o thread. */
	AI->thread_state = THREAD_KILLED; 
#ifdef WIN32
	if (AI->thread != NULL) {
		WaitForSingleObject(AI->thread, INFINITE);
	}
#else
	if (AI->thread != 0) {
		pthread_join(AI->thread, NULL);
		pthread_mutex_destroy(&AI->threadLock);
	}
#endif
	/* Close serial */
	serialClose(&AI->port);
#ifdef MEX_INTERFACE
	/* Allow matlab to clear mex file again */
	mexUnlock();
#endif
	/* Arduino communication now closed, so update connection_state */
	AI->connection_state = ARDUINO_DISCONNECTED;
}

void vsa_arduino_interface_run_step(ArduinoInterface *AI, double *u, double *y ) {
	int i;

	/* 	Limit commands */
	if (u[0]<U_LLIM_RAD_SERVO0) u[0] = U_LLIM_RAD_SERVO0; if (u[0]>U_ULIM_RAD_SERVO0) u[0] = U_ULIM_RAD_SERVO0;
	if (u[1]<U_LLIM_RAD_SERVO1) u[1] = U_LLIM_RAD_SERVO1; if (u[1]>U_ULIM_RAD_SERVO1) u[1] = U_ULIM_RAD_SERVO1;
#ifdef MACCEPA_INTERFACE
#ifdef VARIABLE_DAMPING
	if (u[2]<U_LLIM_DAMPER0   ) u[2] = U_LLIM_DAMPER0   ; if (u[2]>U_ULIM_DAMPER0   ) u[2] = U_ULIM_DAMPER0   ;
#endif
#endif
#ifdef MACCEPA2DOF_INTERFACE
	if (u[2]<U_LLIM_RAD_SERVO2) u[2] = U_LLIM_RAD_SERVO2; if (u[2]>U_ULIM_RAD_SERVO2) u[2] = U_ULIM_RAD_SERVO2;
	if (u[3]<U_LLIM_RAD_SERVO3) u[3] = U_LLIM_RAD_SERVO3; if (u[3]>U_ULIM_RAD_SERVO3) u[3] = U_ULIM_RAD_SERVO3;
#ifdef VARIABLE_DAMPING
	if (u[4]<U_LLIM_DAMPER0   ) u[4] = U_LLIM_DAMPER0   ; if (u[4]>U_ULIM_DAMPER0   ) u[4] = U_ULIM_DAMPER0   ;
	if (u[5]<U_LLIM_DAMPER1   ) u[5] = U_LLIM_DAMPER1   ; if (u[5]>U_ULIM_DAMPER1   ) u[5] = U_ULIM_DAMPER1   ;
#endif
#endif

	/* Convert units from (radians) to (0.5 usec). */
	int M0 = RAD2USEC_SERVO0(u[0]);
	int M1 = RAD2USEC_SERVO1(u[1]);
#ifdef MACCEPA_INTERFACE
#ifdef VARIABLE_DAMPING
	int M2 = DUTY2PWM_DAMPER0(u[2]);
#endif
#endif
#ifdef MACCEPA2DOF_INTERFACE
	int M2 = RAD2USEC_SERVO2(u[2]);
	int M3 = RAD2USEC_SERVO3(u[3]);
#endif

	/* Wait 100 milliseconds for reading from serial. */
	for (i=0;i<TIMEOUT;i++) {
		if (AI->readComplete==1) break;
#ifdef WIN32
		Sleep(1);
#else
		usleep(1000);
#endif
	}
#ifdef MEX_INTERFACE
	if (i>=TIMEOUT) mexErrMsgTxt("Timeout!");
#else
	if (i>=TIMEOUT) fputs("Timeout!",stderr);
#endif

	/* Read arm state, send commands */
	LOCK(AI);
	/* Read measured_state */
#ifdef EDINBURGHVSA_INTERFACE   
   y[0] =  W_POT_JOINT*(double)AI->measured_state[0]+  C_POT_JOINT;
   y[1] =  W_ACC_JOINT*(double)AI->measured_state[1]+  C_ACC_JOINT;
   y[2] = W_POT_SERVO0*(double)AI->measured_state[2]+ C_POT_SERVO0;
   y[3] = W_POT_SERVO1*(double)AI->measured_state[3]+ C_POT_SERVO1;
   y[4] = AI->timestamp;
#endif
#ifdef MACCEPA_INTERFACE   
   y[0] =  W_POT_JOINT*(double)AI->measured_state[0]+  C_POT_JOINT;
   y[1] =  W_ACC_JOINT*(double)AI->measured_state[1]+  C_ACC_JOINT;
   y[2] = W_POT_SERVO0*(double)AI->measured_state[2]+ C_POT_SERVO0;
   y[3] = W_POT_SERVO1*(double)AI->measured_state[3]+ C_POT_SERVO1;
   y[4] = ((5*(double)AI->measured_state[4]/4092)/(CURRENT_RSENSE*(1+(CURRENT_R2/CURRENT_R1))));
   y[5] = AI->timestamp;
#endif
#ifdef MACCEPA2DOF_INTERFACE   
   y[ 0] = W_POT_JOINT0*(double)AI->measured_state[0] + C_POT_JOINT0;
   y[ 1] = W_POT_JOINT1*(double)AI->measured_state[1] + C_POT_JOINT1;
   y[ 2] = W_POT_SERVO0*(double)AI->measured_state[2] + C_POT_SERVO0;
   y[ 3] = W_POT_SERVO1*(double)AI->measured_state[3] + C_POT_SERVO1;
   y[ 4] = W_POT_SERVO2*(double)AI->measured_state[4] + C_POT_SERVO2;
   y[ 5] = W_POT_SERVO3*(double)AI->measured_state[5] + C_POT_SERVO3;
#ifdef CURRENT_SENSING
   y[ 6] = ((5*(double)AI->measured_state[6]/4092)/(CURRENT_RSENSE*(1+(CURRENT_R2/CURRENT_R1))));
   y[ 7] = ((5*(double)AI->measured_state[7]/4092)/(CURRENT_RSENSE*(1+(CURRENT_R2/CURRENT_R1))));
   y[ 8] = ((5*(double)AI->measured_state[8]/4092)/(CURRENT_RSENSE*(1+(CURRENT_R2/CURRENT_R1))));
   y[ 9] = ((5*(double)AI->measured_state[9]/4092)/(CURRENT_RSENSE*(1+(CURRENT_R2/CURRENT_R1))));
   y[10] = AI->timestamp;
#else
   y[6] = AI->timestamp;
#endif
#endif
	/* Write values to command buffer. */
	AI->command_buffer[ 0] = RECEIVE_HEADER;   
	AI->command_buffer[ 1] = M0 & 0xFE;  /* switch off least significant bit so it can never be equal to the header=0xFF */
	AI->command_buffer[ 2] = M0 >> 8; 
	AI->command_buffer[ 3] = M1 & 0xFE;
	AI->command_buffer[ 4] = M1 >> 8;
#ifdef MACCEPA_INTERFACE
#ifdef VARIABLE_DAMPING
	AI->command_buffer[ 5] = M2 & 0xFE;  /* switch off least significant bit so it can never be equal to the header=0xFF */
	AI->command_buffer[ 6] = M2 >> 8; 
#endif
#endif
#ifdef MACCEPA2DOF_INTERFACE   
	AI->command_buffer[ 5] = M2 & 0xFE;
	AI->command_buffer[ 6] = M2 >> 8;
	AI->command_buffer[ 7] = M3 & 0xFE;
	AI->command_buffer[ 8] = M3 >> 8;
#ifdef VARIABLE_DAMPING
	AI->command_buffer[ 9] = (int)u[4];
	AI->command_buffer[10] = 0x00;
	AI->command_buffer[11] = (int)u[5];
	AI->command_buffer[12] = 0x00;
#ifdef MAGNET /* if damping and magnet control */
	AI->command_buffer[13] = (int)u[6];
	AI->command_buffer[14] = 0x00;
#endif
#else
#ifdef MAGNET /* if just magnet control */
	AI->command_buffer[ 9] = (int)u[6];
	AI->command_buffer[10] = 0x00;
#endif
#endif
#endif
	AI->readComplete  = 0;
	UNLOCK(AI);
}

void vsa_arduino_interface_read ( ArduinoInterface *AI, double *y ) {
	int i;

	/* Wait for next serial read. */
	for (i=0;i<TIMEOUT;i++) {
		if (AI->readComplete==1) break;
      #ifdef WIN32
      Sleep(1);
      #else
      usleep(1000);
      #endif
   }
   if (i>=TIMEOUT) fputs("Timeout!",stderr);
   
   LOCK(AI);
#ifdef EDINBURGHVSA_INTERFACE   
   y[0] =  W_POT_JOINT*(double)AI->measured_state[0]+  C_POT_JOINT;
   y[1] =  W_ACC_JOINT*(double)AI->measured_state[1]+  C_ACC_JOINT;
   y[2] = W_POT_SERVO0*(double)AI->measured_state[2]+ C_POT_SERVO0;
   y[3] = W_POT_SERVO1*(double)AI->measured_state[3]+ C_POT_SERVO1;
   y[4] = AI->timestamp;
#endif
#ifdef MACCEPA_INTERFACE   
   y[0] =  W_POT_JOINT*(double)AI->measured_state[0]+  C_POT_JOINT;
   y[1] =  W_ACC_JOINT*(double)AI->measured_state[1]+  C_ACC_JOINT;
   y[2] = W_POT_SERVO0*(double)AI->measured_state[2]+ C_POT_SERVO0;
   y[3] = W_POT_SERVO1*(double)AI->measured_state[3]+ C_POT_SERVO1;
   y[4] = ((5*(double)AI->measured_state[4]/4092)/(CURRENT_RSENSE*(1+(CURRENT_R2/CURRENT_R1))));
   y[5] = AI->timestamp;
#endif
#ifdef MACCEPA2DOF_INTERFACE   
   y[0] = W_POT_JOINT0*(double)AI->measured_state[0] + C_POT_JOINT0;
   y[1] = W_POT_JOINT1*(double)AI->measured_state[1] + C_POT_JOINT1;
   y[2] = W_POT_SERVO0*(double)AI->measured_state[2] + C_POT_SERVO0;
   y[3] = W_POT_SERVO1*(double)AI->measured_state[3] + C_POT_SERVO1;
   y[4] = W_POT_SERVO2*(double)AI->measured_state[4] + C_POT_SERVO2;
   y[5] = W_POT_SERVO3*(double)AI->measured_state[5] + C_POT_SERVO3;
   y[6] = AI->timestamp;
#endif
   AI->readComplete  = 0;
   UNLOCK(AI);
}

void vsa_arduino_interface_write ( ArduinoInterface *AI, double * u ) { 

	/* 	Limit commands */
	if (u[0]<U_LLIM_RAD_SERVO0) u[0] = U_LLIM_RAD_SERVO0; if (u[0]>U_ULIM_RAD_SERVO0) u[0] = U_ULIM_RAD_SERVO0;
	if (u[1]<U_LLIM_RAD_SERVO1) u[1] = U_LLIM_RAD_SERVO1; if (u[1]>U_ULIM_RAD_SERVO1) u[1] = U_ULIM_RAD_SERVO1;
#ifdef MACCEPA_INTERFACE
#ifdef VARIABLE_DAMPING
	if (u[2]<U_LLIM_DAMPER0   ) u[2] = U_LLIM_DAMPER0   ; if (u[2]>U_ULIM_DAMPER0   ) u[2] = U_ULIM_DAMPER0   ;
#endif
#endif
#ifdef MACCEPA2DOF_INTERFACE
	if (u[2]<U_LLIM_RAD_SERVO2) u[2] = U_LLIM_RAD_SERVO2; if (u[2]>U_ULIM_RAD_SERVO2) u[2] = U_ULIM_RAD_SERVO2;
	if (u[3]<U_LLIM_RAD_SERVO3) u[3] = U_LLIM_RAD_SERVO3; if (u[3]>U_ULIM_RAD_SERVO3) u[3] = U_ULIM_RAD_SERVO3;
#ifdef VARIABLE_DAMPING
	if (u[4]<U_LLIM_DAMPER0   ) u[4] = U_LLIM_DAMPER0   ; if (u[4]>U_ULIM_DAMPER0   ) u[4] = U_ULIM_DAMPER0   ;
	if (u[5]<U_LLIM_DAMPER1   ) u[5] = U_LLIM_DAMPER1   ; if (u[5]>U_ULIM_DAMPER1   ) u[5] = U_ULIM_DAMPER1   ;
#endif
#endif

	/* Convert units from (radians) to (0.5 usec). */
	int M0 = RAD2USEC_SERVO0(u[0]);
	int M1 = RAD2USEC_SERVO1(u[1]);
#ifdef MACCEPA_INTERFACE
#ifdef VARIABLE_DAMPING
	int M2 = DUTY2PWM_DAMPER0(u[2]);
#endif
#endif
#ifdef MACCEPA2DOF_INTERFACE
	int M2 = RAD2USEC_SERVO2(u[2]);
	int M3 = RAD2USEC_SERVO3(u[3]);
#endif

	LOCK(AI);
	AI->command_buffer[ 0] = RECEIVE_HEADER;   
	AI->command_buffer[ 1] = M0 & 0xFE; 
	AI->command_buffer[ 2] = M0 >> 8; 
	AI->command_buffer[ 3] = M1 & 0xFE;
	AI->command_buffer[ 4] = M1 >> 8;
#ifdef MACCEPA_INTERFACE
#ifdef VARIABLE_DAMPING
	AI->command_buffer[ 5] = M2 & 0xFE;
	AI->command_buffer[ 6] = M2 >> 8;
#endif
#endif
#ifdef MACCEPA2DOF_INTERFACE
	AI->command_buffer[ 5] = M2 & 0xFE;
	AI->command_buffer[ 6] = M2 >> 8;
	AI->command_buffer[ 7] = M3 & 0xFE;
	AI->command_buffer[ 8] = M3 >> 8;
#ifdef VARIABLE_DAMPING
	AI->command_buffer[ 9] = (int)u[4];
	AI->command_buffer[10] = 0x00;
	AI->command_buffer[11] = (int)u[5];
	AI->command_buffer[12] = 0x00;
#ifdef MAGNET /* if damping and magnet control */
	AI->command_buffer[13] = (int)u[6];
	AI->command_buffer[14] = 0x00;
#endif
#else
#ifdef MAGNET /* if just magnet control */
	AI->command_buffer[ 9] = (int)u[6];
	AI->command_buffer[10] = 0x00;
#endif
#endif
#endif
	serialWrite(&AI->port, RECEIVE_LENGTH+1, AI->command_buffer);
	UNLOCK(AI);
}

void vsa_arduino_interface_write_usec ( ArduinoInterface *AI, int * u ) { 
	int i,j=1;
	LOCK(AI);
	AI->command_buffer[0] = RECEIVE_HEADER;
   	for ( i = 0; i < DIMU; i += 1 ) {
		AI->command_buffer[j  ] = u[i] & 0xFE; 
		AI->command_buffer[j+1] = u[i] >> 8; 
		j+=2;
   	}
#ifdef MACCEPA2DOF_INTERFACE
#ifdef VARIABLE_DAMPING
	AI->command_buffer[ 9]=u[4];
	AI->command_buffer[10]=0x00;
	AI->command_buffer[11]=u[5];
	AI->command_buffer[12]=0x00;
#ifdef MAGNET /* if damping and magnet control */
	AI->command_buffer[13] = u[6];
	AI->command_buffer[14] = 0x00;
#endif
#else
#ifdef MAGNET /* if just magnet control */
	AI->command_buffer[ 9] = u[6];
	AI->command_buffer[10] = 0x00;
#endif
#endif
#endif
	serialWrite(&AI->port, RECEIVE_LENGTH+1, AI->command_buffer);
	UNLOCK(AI);
}

void vsa_arduino_interface_read_adc ( ArduinoInterface *AI, int * y) {
	int i;

	for (i=0;i<TIMEOUT;i++) {
		if (AI->readComplete==1) break;
#ifdef WIN32
		Sleep(1);
#else
		usleep(1000);
#endif
	}
	if (i>=TIMEOUT) fputs("Timeout!",stderr);

	LOCK(AI);
	for (i=0;i<DIMY;i++) y[i] = AI->measured_state[i];
	AI->readComplete  = 0;
	UNLOCK(AI);
}

