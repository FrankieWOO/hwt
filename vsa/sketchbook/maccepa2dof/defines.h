/** 
 * \file sketchbook/maccepa2dof/defines.h
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \date 27/12/10 13:05:35 GMT
 * \ingroup MACCEPA2DOF
 *
 * \brief Defines for maccepa2dof.pde.
 */
#ifndef __maccepa2dof_defines_h
#define __maccepa2dof_defines_h

#define VARIABLE_DAMPING
#define MAGNET

/* \brief Dimension of command vector. */
#ifdef VARIABLE_DAMPING
#ifdef MAGNET
#define DIMU 7
#else
#define DIMU 6
#endif
#else
#ifdef MAGNET
#define DIMU 5
#else
#define DIMU 4
#endif
#endif

/* \brief Dimension of observation vector (number of sensors). */
#ifdef CURRENT_SENSING
#define DIMY 10
#else
#define DIMY 6
#endif

/** \brief Baudrate used for serial communication. */
#define BAUDRATE 38400
/** \brief No. of bytes in 'transmit' bit-string. */
#define TRANSMIT_LENGTH 2*(DIMY+1)
/** \brief First header byte of 'transmit' bit-string. */
#define TRANSMIT_HEADER0 0xFF
/** \brief Second header byte of 'transmit' bit-string. */
#define TRANSMIT_HEADER1 0xFF
/** \brief No. of bytes in 'receive' bit-string. */
#define RECEIVE_LENGTH 2*DIMU
/** \brief Header byte of 'receive' bit-string. */
#define RECEIVE_HEADER 0xFF

/** \brief PWM output pin on Arduino for controlling Servo 0. */
#define OUTPUT_PIN_SERVO0 11
/** \brief PWM output pin on Arduino for controlling Servo 1. */
#define OUTPUT_PIN_SERVO1 12
/** \brief PWM output pin on Arduino for controlling Servo 2. */
#define OUTPUT_PIN_SERVO2 5
/** \brief PWM output pin on Arduino for controlling Servo 3. */
#define OUTPUT_PIN_SERVO3 2
#ifdef MAGNET
/** \brief Output pin on Arduino for controlling magnet. */
#define OUTPUT_PIN_MAGNET 10
#endif

/** \brief Analogue input pin on Arduino for reading base joint potentiometer. */
#define INPUT_PIN_JOINT0_POT 2
/** \brief Analogue input pin on Arduino for reading second joint potentiometer. */
#define INPUT_PIN_JOINT1_POT 12
/** \brief Analogue input pin on Arduino for reading Servo 0 potentiometer. */
#define INPUT_PIN_SERVO0_POT 0
/** \brief Analogue input pin on Arduino for reading Servo 1 potentiometer. */
#define INPUT_PIN_SERVO1_POT 5
/** \brief Analogue input pin on Arduino for reading Servo 2 potentiometer. */
#define INPUT_PIN_SERVO2_POT 10
/** \brief Analogue input pin on Arduino for reading Servo 3 potentiometer. */
#define INPUT_PIN_SERVO3_POT 15

/** \brief Analogue input pin on Arduino for reading Servo0 current sensor. */
#define INPUT_PIN_SERVO0_CURRENT_SENSOR 14
/** \brief Analogue input pin on Arduino for reading Servo1 current sensor. */
#define INPUT_PIN_SERVO1_CURRENT_SENSOR 9
/** \brief Analogue input pin on Arduino for reading Servo2 current sensor. */
#define INPUT_PIN_SERVO2_CURRENT_SENSOR 4
/** \brief Analogue input pin on Arduino for reading Servo3 current sensor. */
#define INPUT_PIN_SERVO3_CURRENT_SENSOR 1



/**
 * \brief Minimum position command for Servo 0 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 0 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has minimum
 * angle corresponding to 600 microseconds (so this
 * should be around 1200).
 */
#define U_MIN_SERVO0 1200
/**
 * \brief Minimum position command for Servo 1 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 0 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has minimum
 * angle corresponding to 600 microseconds (so this
 * should be around 1200).
 */
#define U_MIN_SERVO1 1100
/**
 * \brief Minimum position command for Servo 2 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 0 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has minimum
 * angle corresponding to 600 microseconds (so this
 * should be around 1200).
 */
#define U_MIN_SERVO2 1200
/**
 * \brief Minimum position command for Servo 3 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 0 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has minimum
 * angle corresponding to 600 microseconds (so this
 * should be around 1200).
 */
#define U_MIN_SERVO3 1200
#ifdef VARIABLE_DAMPING
/**
 * \brief Minimum wiper position command for Damper 0 digipot
 */
#define U_MIN_DAMPER0 0
/**
 * \brief Minimum wiper position command for Damper 1 digipot
 */
#define U_MIN_DAMPER1 0
#endif
#ifdef MAGNET
/**
 * \brief Magnet 'off' command
 */
#define U_MAGNET_OFF 0x00
#endif

/**
 * \brief Maximum commanded position to Servo 0 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 180 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has maximum
 * angle 2400 microseconds (so this should be around 4800).
 */
#define U_MAX_SERVO0 4800
/**
 * \brief Maximum commanded position to Servo 1 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 180 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has maximum
 * angle 2400 microseconds (so this should be around 4800).
 */
#define U_MAX_SERVO1 4700
/**
 * \brief Maximum commanded position to Servo 2 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 180 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has maximum
 * angle 2400 microseconds (so this should be around 4800).
 */
#define U_MAX_SERVO2 4800
/**
 * \brief Maximum commanded position to Servo 3 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 180 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has maximum
 * angle 2400 microseconds (so this should be around 4800).
 */
#define U_MAX_SERVO3 4700
#ifdef VARIABLE_DAMPING
/**
 * \brief Maximum wiper position command for Damper 0 digipot
 */
#define U_MAX_DAMPER0 63
/**
 * \brief Maximum wiper position command for Damper 1 digipot
 */
#define U_MAX_DAMPER1 63
#endif
#ifdef MAGNET
/**
 * \brief Magnet 'on' command
 */
#define U_MAGNET_ON 0x01
#endif

/**
 * \brief Midrange position command for Servo 0 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 90 degrees and may
 * need to be adjusted according to the actual servo.
 */
#define U_MID_SERVO0 (int)(0.5*(U_MAX_SERVO0-U_MIN_SERVO0)+U_MIN_SERVO0)
/**
 * \brief Midrange position command for Servo 2 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 90 degrees and may
 * need to be adjusted according to the actual servo.
 */
#define U_MID_SERVO2 (int)(0.5*(U_MAX_SERVO2-U_MIN_SERVO2)+U_MIN_SERVO2)

/**
 * \brief Initial position command for Servo 0 (in
 * units of 0.5 microseconds).
 */
#define U_INIT_SERVO0 U_MID_SERVO0
/**
 * \brief Initial position command for Servo 1 (in
 * units of 0.5 microseconds).
 */
#define U_INIT_SERVO1 U_MIN_SERVO1
/**
 * \brief Initial position command for Servo 2 (in
 * units of 0.5 microseconds).
 */
#define U_INIT_SERVO2 U_MID_SERVO2
/**
 * \brief Initial position command for Servo 3 (in
 * units of 0.5 microseconds).
 */
#define U_INIT_SERVO3 U_MIN_SERVO3
#ifdef VARIABLE_DAMPING
/**
 * \brief Initial wiper position command for Damper 0 digipot
 */
#define U_INIT_DAMPER0 U_MIN_DAMPER0
/**
 * \brief Initial wiper position command for Damper 1 digipot
 */
#define U_INIT_DAMPER1 U_MIN_DAMPER1
#endif
#ifdef MAGNET
/**
 * \brief Initial magnet command
 */
#define U_INIT_MAGNET U_MAGNET_ON
#endif

/** \brief Maximum commanded position to Servo 0 in units of radiens. */
#define U_MAX_RAD_SERVO0 .5*M_PI
/** \brief Maximum commanded position to Servo 1 in units of radiens. */
#define U_MAX_RAD_SERVO1 M_PI
/** \brief Maximum commanded position to Servo 2 in units of radiens. */
#define U_MAX_RAD_SERVO2 .5*M_PI
/** \brief Maximum commanded position to Servo 3 in units of radiens. */
#define U_MAX_RAD_SERVO3 M_PI

/** \brief Minimum commanded position to Servo 0 in units of radiens. */
#define U_MIN_RAD_SERVO0 -.5*M_PI
/** \brief Minimum commanded position to Servo 1 in units of radiens. */
#define U_MIN_RAD_SERVO1 0
/** \brief Minimum commanded position to Servo 2 in units of radiens. */
#define U_MIN_RAD_SERVO2 -.5*M_PI
/** \brief Minimum commanded position to Servo 3 in units of radiens. */
#define U_MIN_RAD_SERVO3 0

/**
 * \brief Upper command limit for Servo 0 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * maximum position that the servo can reach (as
 * defined by U_MAX_SERVO0).
 */
#define U_ULIM_RAD_SERVO0 U_MAX_RAD_SERVO0
/**
 * \brief Upper command limit for Servo 1 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * maximum position that the servo can reach (as
 * defined by U_MAX_SERVO1).
 */
#define U_ULIM_RAD_SERVO1 .5*M_PI
/**
 * \brief Upper command limit for Servo 2 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * maximum position that the servo can reach (as
 * defined by U_MAX_SERVO2).
 */
#define U_ULIM_RAD_SERVO2 U_MAX_RAD_SERVO2
/**
 * \brief Upper command limit for Servo 3 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * maximum position that the servo can reach (as
 * defined by U_MAX_SERVO3).
 */
#define U_ULIM_RAD_SERVO3 .5*M_PI
#ifdef VARIABLE_DAMPING
/**
 * \brief Upper command limit for Damper 0 
 */
#define U_ULIM_DAMPER0 63
/**
 * \brief Upper command limit for Damper 1 
 */
#define U_ULIM_DAMPER1 63
#endif

/** 
 * \brief Lower command limit for Servo 0 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * minimum position that the servo can reach (as
 * defined by U_MIN_SERVO0). 
 */
#define U_LLIM_RAD_SERVO0 U_MIN_RAD_SERVO0
/** 
 * \brief Lower command limit for Servo 1 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * minimum position that the servo can reach (as
 * defined by U_MIN_SERVO1). 
 */
#define U_LLIM_RAD_SERVO1 0
/**
 * \brief Lower command limit for Servo 2 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * minimum position that the servo can reach (as
 * defined by U_MIN_SERVO2). 
 */
#define U_LLIM_RAD_SERVO2 U_MIN_RAD_SERVO2
/**
 * \brief Lower command limit for Servo 3 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * minimum position that the servo can reach (as
 * defined by U_MIN_SERVO3). 
 */
#define U_LLIM_RAD_SERVO3 0
#ifdef VARIABLE_DAMPING
/**
 * \brief Lower command limit for Damper 0 
 */
#define U_LLIM_DAMPER0 0
/**
 * \brief Lower command limit for Damper 1 
 */
#define U_LLIM_DAMPER1 0
#endif

/**
 * \brief Scaling factor for converting raw pot readings from base joint to angle (in radiens). 
 */
#define W_POT_JOINT0 (M_PI/2)/(3192-2080)
/** 
 * \brief Scaling factor for converting raw pot readings from second joint to angle (in radiens). 
 */
#define W_POT_JOINT1 (M_PI/2)/(3092-2008)
/**
 * \brief Scaling factor for converting raw pot
 * readings from Servo 0 to the motor angle (in
 * radiens). 
 */
#define W_POT_SERVO0 M_PI/(2536-161)
/**
 * \brief Scaling factor for converting raw pot
 * readings from Servo 1 to the motor angle (in
 * radiens). 
 */
#define W_POT_SERVO1 M_PI/(219-2556)
/**
 * \brief Scaling factor for converting raw pot
 * readings from Servo 2 to the motor angle (in
 * radiens). 
 */
#define W_POT_SERVO2 -M_PI/(2641-272)
/**
 * \brief Scaling factor for converting raw pot
 * readings from Servo 3 to the motor angle (in
 * radiens). 
 */
#define W_POT_SERVO3 M_PI/(191-2544)

/**
 * \brief Offset factor for converting raw pot
 * readings from base joint to angle (in radiens). 
 */
#define C_POT_JOINT0 -W_POT_JOINT0*(2080)
/**
 * \brief Offset factor for converting raw pot
 * readings from second joint to angle (in radiens). 
 */
#define C_POT_JOINT1 -W_POT_JOINT1*(2008)
/** 
 * \brief Offset factor for converting raw pot
 * readings from Servo 0 to the motor angle (in
 * radiens). 
 */
#define C_POT_SERVO0 (M_PI/2-W_POT_SERVO0*2536)
/**
 * \brief Offset factor for converting raw pot
 * readings from Servo 1 to the motor angle (in
 * radiens). 
 */
#define C_POT_SERVO1 -W_POT_SERVO1*2556
/**
 * \brief Offset factor for converting raw pot
 * readings from Servo 2 to the motor angle (in
 * radiens). 
 */
#define C_POT_SERVO2 (-M_PI/2-W_POT_SERVO2*2641)
/**
 * \brief Offset factor for converting raw pot
 * readings from Servo 3 to the motor angle (in
 * radiens). 
 */
#define C_POT_SERVO3 -W_POT_SERVO3*2544

/** \brief Resistance of R_sense resistor (Ohms) in power sensing circuit. */
#define CURRENT_RSENSE 0.1

/** \brief Resistance of R1 resistor (Ohms) in power sensing circuit. */
#define CURRENT_R1 4200.0

/** \brief Resistance of R2 resistor (Ohms) in power sensing circuit. */
#define CURRENT_R2 21800.0

#endif

