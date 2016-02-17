/** 
 * \file vsa_mex_interface.c
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \date 04/04/11 13:41:40
 * \brief Matlab MEX gateway for controlling/reading sensor values from the variable stiffness robots in the lab, via the Arduino serial interface.
 */
#include <ctype.h>
#include <mex.h>
#include <matrix.h>
#include <vsa_arduino_interface.h>

/** \brief Arduino interface. */
ArduinoInterface AI;

/** \brief Mex interface usage message. */
static char usage_msg[]=
#ifdef EDINBURGHVSA_INTERFACE
"Usage of Edinburgh VSA MEX interface:\n" \
"  edinburghvsa('I', device) opens the Arduino board on the specified serial port device,\n" \
"                       e.g., /dev/ttyUSB0 on Linux or COM1 on Windows. \n" \
"  edinburghvsa('C')         closes the connection.\n" \
"  y = edinburghvsa(u)       move motors to u(1),u(2) and read joint angle,\n" \
"                       acceleration, motor 1 pot, motor 2 pot and timestamp.";
#endif
#ifdef MACCEPA_INTERFACE
"Usage of MACCEPA MEX interface:\n" \
"  maccepa('I', device) opens the Arduino board on the specified serial port device,\n" \
"                       e.g., /dev/ttyUSB0 on Linux or COM1 on Windows. \n" \
"  maccepa('C')         closes the connection.\n" \
"  y = maccepa(u)       move motors to u(1),u(2) and read joint angle,\n" \
"                       acceleration, motor positions, motor current and timestamp.\n";
#endif
#ifdef MACCEPA2DOF_INTERFACE
"Usage of 2-DOF MACCEPA MEX interface:\n" \
"  maccepa2dof('I', device) opens the Arduino board on the specified serial port device,\n" \
"                       e.g., /dev/ttyUSB0 on Linux or COM1 on Windows. \n" \
"  maccepa2dof('C')         closes the connection.\n" \
"  y = maccepa2dof(u)   move motors to u(1),...,u(4), damper pots to u(5),u(6) and read joint angles,\n" \
"                       motor positions, motor currents and timestamp.\n";
#endif

/** 
 * \brief Mex gateway function. 
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	int i;

	/* Not enough arguments. Print usage message and exit. */
	if (nrhs < 1) {
		printf(usage_msg);
		return;
	}

	/* First argument is a char (-> connect/disconnect command). */
	if (mxIsChar(prhs[0])) {
		char action[2];
		char device[128];
		double t;

		mxGetString(prhs[0], action, 2);

		switch(toupper(action[0])) {
			case 'I': /* First char is an 'I' -> open arduino */
				if (nrhs < 2 || !mxIsChar(prhs[1])) { /* check the port is specified. */
					printf(usage_msg);
					return;
				}
				mxGetString(prhs[1], device, 128); /* get port name */
				vsa_arduino_interface_init(&AI, device); /* open arduino communication */
				return;
			case 'C': /* First char is a 'C' -> close arduino */
				vsa_arduino_interface_close(&AI);                    /* close arduino communication */
				return;
			default:
				printf(usage_msg);
				return;
		}
	}

	/* First argument is a double. */
	int nEl = mxGetM(prhs[0])*mxGetN(prhs[0]);

	if (nEl == DIMU) {
		double *pm = mxGetPr(prhs[0]);

		vsa_arduino_interface_check(&AI); /* Check that serial connection has been made with Arduino. */

		plhs[0] = mxCreateDoubleMatrix(DIMY+1,1,mxREAL); /* Create output vector */

#ifdef MACCEPA2DOF_INTERFACE
		double u[7]; 
		for ( i = 0; i < 7; i += 1 ) { u[i] = 0; }
		u[0] = pm[0];
		u[1] = pm[1];
		u[2] = pm[2];
		u[3] = pm[3];
#ifdef VARIABLE_DAMPING
		u[4] = pm[4];
		u[5] = pm[5];
#ifdef MAGNET /* if damping and magnet control */
		u[6] = pm[6];
#endif
#else
#ifdef MAGNET /* if just magnet control */
		u[4] = 0;
		u[5] = 0;
		u[6] = pm[4];
#endif
#endif
#ifdef MEX_RAW_VALUES
		/* write raw values to servos (units of PWM microseconds, no command limits!) */
		vsa_arduino_interface_write_usec(&AI, (int*)u);
		/* read raw ADC values */
		vsa_arduino_interface_read_adc  (&AI, (int*)mxGetPr(plhs[0]));
#else
		vsa_arduino_interface_run_step(&AI, u, mxGetPr(plhs[0])); /* Send command to motors. */
#endif
#else
		double u[DIMU]; for ( i = 0; i < DIMU; i += 1 ) { u[i] = pm[i]; }
		vsa_arduino_interface_run_step(&AI, u, mxGetPr(plhs[0])); /* Send command to motors. */
#endif
	} else {
		printf(usage_msg);
	}
}
