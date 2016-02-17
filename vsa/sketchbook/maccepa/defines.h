/** 
 * \file sketchbook/maccepa/defines.h
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \ingroup MACCEPA
 *
 * \brief Defines for maccepa.pde.
 * \sa <a href="../m2html/MACCEPA/identify_maccepa.html">identify_maccepa.m</a>
 */
#ifndef __maccepa_defines_h
#define __maccepa_defines_h

/* #define VARIABLE_DAMPING */

/** \brief Dimensionality of joint space. */
#define DIMQ 1

/* \brief Dimensionality of command vector. */
#ifdef VARIABLE_DAMPING
#define DIMU 3
#else
#define DIMU 2
#endif

/* \brief Dimensionality of observation vector (number of sensors). */
#define DIMY 5

/** \brief Baudrate used for serial communication. */
/* orig 38400*/
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
#define OUTPUT_PIN_SERVO0 9
/** \brief PWM output pin on Arduino for controlling Servo 1. */
#define OUTPUT_PIN_SERVO1 10
#ifdef  VARIABLE_DAMPING
/** \brief PWM output pin on Arduino for controlling Damper 0. */
#define OUTPUT_PIN_DAMPER0 2
#endif     /* -----  not VARIABLE_DAMPING  ----- */

/** \brief First analogue input pin on Arduino for reading joint position sensor (potentiometer). */
#define INPUT_PIN_JOINT_POT0 0
/** \brief Second analogue input pin on Arduino for reading joint position sensor (potentiometer). */
#define INPUT_PIN_JOINT_POT1 1
/** \brief Analogue input pin on Arduino for reading power consumption. */
#define INPUT_PIN_CURRENT    2
/** \brief Analogue input pin on Arduino for reading joint accelerometer. */
#define INPUT_PIN_JOINT_ACC  3
/** \brief Analogue input pin on Arduino for reading Servo 0 potentiometer. */
#define INPUT_PIN_SERVO0_POT 4
/** \brief Analogue input pin on Arduino for reading Servo 1 potentiometer. */
#define INPUT_PIN_SERVO1_POT 5

/**
 * \brief Minimum position command for Servo 0 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 0 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has minimum
 * angle corresponding to 600 microseconds (so this
 * should be around 1200).
 * Alex: For the HS-7950TH the tested min command 
 * is 1506 , for the HSR-5990TG it was 1350
 */
#define U_MIN_SERVO0 1506
/**
 * \brief Minimum position command for Servo 1 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 0 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has minimum
 * angle corresponding to 600 microseconds (so this
 * should be around 1200).
 * Alex: For HS-7950TH the tested min command is 1506
 * The HSR-5990TG had originally a value of 1100
 */
#define U_MIN_SERVO1 1506
#ifdef  VARIABLE_DAMPING
/**
 * \brief Minimum PWM command for Damper 0
 */
#define U_MIN_DAMPER0 0
#endif     /* -----  not VARIABLE_DAMPING  ----- */

/**
 * \brief Maximum commanded position to Servo 0 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 180 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has maximum
 * angle 2400 microseconds (so this should be around 4800).
 * Alex: The HS-7950TH has a tested max command of 4521
 * The HSR-5990TG had originally a value of 4690
 */
#define U_MAX_SERVO0 4521
/**
 * \brief Maximum commanded position to Servo 1 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 180 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has maximum
 * angle 2400 microseconds (so this should be around 4800).
 * Alex: The HS-7950TH has a tested max command of 4521
 * The HSR-5990TG had originally a value of 4650
*/
#define U_MAX_SERVO1 4521
#ifdef  VARIABLE_DAMPING
/**
 * \brief Maximum PWM command for Damper 0
 */
#define U_MAX_DAMPER0 255
#endif     /* -----  not VARIABLE_DAMPING  ----- */

/**
 * \brief Midrange position command for Servo 0 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 90 degrees and may
 * need to be adjusted according to the actual servo.
 */
#define U_MID_SERVO0 (int)(0.5*(U_MAX_SERVO0-U_MIN_SERVO0)+U_MIN_SERVO0)

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
#ifdef  VARIABLE_DAMPING
/**
 * \brief Initial PWM command for Damper 0
 */
#define U_INIT_DAMPER0 U_MIN_DAMPER0
#endif     /* -----  not VARIABLE_DAMPING  ----- */

/** \brief Maximum commanded position to Servo 0 in units of radiens. */
#define U_MAX_RAD_SERVO0 .5*M_PI
/** \brief Maximum commanded position to Servo 1 in units of radiens. */
#define U_MAX_RAD_SERVO1 M_PI
/** \brief Maximum commanded duty cycle for Damper 0. */
#define U_MAX_DUTY_DAMPER0 1

/** \brief Minimum commanded position to Servo 0 in units of radiens. */
#define U_MIN_RAD_SERVO0 -.5*M_PI
/** \brief Minimum commanded position to Servo 1 in units of radiens. */
#define U_MIN_RAD_SERVO1 0
/** \brief Minimum commanded duty cycle for Damper 0. */
#define U_MIN_DUTY_DAMPER0 0

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
 *
 * \note With the large drum configuration 
 * we can only move the servo half its range as
 * otherwise the spring comes off the runner.
 * Alex: for the new robot we can use the max
 * instead of the old M_PI/2 
*/
#define U_ULIM_RAD_SERVO1 U_MAX_RAD_SERVO1
#ifdef VARIABLE_DAMPING
/**
 * \brief Upper command limit for Damper 0 
 */
#define U_ULIM_DAMPER0 1
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
#define U_LLIM_RAD_SERVO1 U_MIN_RAD_SERVO1
#ifdef VARIABLE_DAMPING
/**
 * \brief Lower command limit for Damper 0 
 */
#define U_LLIM_DAMPER0 0
#endif

/**
 * \brief Scaling factor for converting raw joint potentiometer readings to angles (radiens).
 *
 * \note If using the Matlab system-id scripts,
 * set this to 1, then run
 * identify_maccepa_joint_potentiometer.m to calculate
 * this value with linear regression. was 0.000923775551
 */
#define W_POT_JOINT 0.0014
/** 
 * \brief Scaling factor for converting raw joint
 * accelerometer readings to anglular accleration
 * (radiens per second squared).
 *
 * \note If using the Matlab system-id scripts,
 * set this to 1, then run
 * identify_maccepa_joint_accelerometer.m to calculate
 * this value with linear regression.
 */
#define W_ACC_JOINT -0.069275180022
/**
 * \brief Scaling factor for converting raw pot
 * readings from Servo 0 to the motor angle (in
 * radiens). 
 *
 * \note If using the Matlab system-id scripts,
 * set this to 1, then run
 * identify_maccepa_servo_potentiometers.m to
 * calculate this value with linear regression. 
 */
#define W_POT_SERVO0 M_PI/(2350-309)
/**
 * \brief Scaling factor for converting raw pot
 * readings from Servo 1 to the motor angle (in
 * radiens). 
 *
 * \note If using the Matlab system-id scripts,
 * set this to 1, then run
 * identify_maccepa_servo_potentiometers.m to
 * calculate this value with linear regression. 
 */
#define W_POT_SERVO1 (M_PI/2)/(1345-2417)

/**
 * \brief Offset factor for converting raw joint
 * potentiometer readings to angle (radiens).
 *
 * \note If using the Matlab system-id scripts, set
 * this to 1, then run
 * identify_maccepa_joint_potentiometer.m to calculate
 * this value with linear regression. was -3.928447906725
 */
#define C_POT_JOINT -2.8438
/**
 * \brief Offset factor for converting raw joint
 * accelerometer readings to anglular acceleration
 * (radiens per second squared).
 *
 * \note If using the Matlab system-id scripts, set
 * this to 1, then run
 * identify_maccepa_joint_accelerometer.m to calculate
 * this value with linear regression.
 */
#define C_ACC_JOINT 144.902948684437
/**
 * \brief Offset factor for converting raw pot
 * readings from Servo 0 to the motor angle (in
 * radiens). 
 *
 * \note If using the Matlab system-id scripts,
 * set this to 0, then run
 * identify_maccepa_servo_potentiometers.m to
 * calculate this value with linear regression.
 */
#define C_POT_SERVO0 (M_PI/2-W_POT_SERVO0*2350)/*-2.070907112358*//*(M_PI/2-W_POT_SERVO0*2500)*//*-1.8980*//*-2.070907112358*/
/**
 * \brief Offset factor for converting raw pot
 * readings from Servo 1 to the motor angle (in
 * radiens). 
 *
 * \note If using the Matlab system-id scripts,
 * set this to 0, then run
 * identify_maccepa_servo_potentiometers.m to
 * calculate this value with linear regression.
 */
#define C_POT_SERVO1 -W_POT_SERVO1*2417

/** \brief FIR filter paramters for servo 0. */
#define SERVO0_FIR_B0 -0.217356409267
#define SERVO0_FIR_B1 -0.063287872747
#define SERVO0_FIR_B2 0.091782750400
#define SERVO0_FIR_B3 0.245400105129
#define SERVO0_FIR_B4 0.395131847134
#define SERVO0_FIR_B5 0.538607156071

/** \brief FIR filter paramters for servo 1. */
#define SERVO1_FIR_B0 -0.217356409267
#define SERVO1_FIR_B1 -0.063287872747
#define SERVO1_FIR_B2 0.091782750400
#define SERVO1_FIR_B3 0.245400105129
#define SERVO1_FIR_B4 0.395131847134
#define SERVO1_FIR_B5 0.538607156071

/** \brief Dimensionality of FIR filters for modelling servo motor dynamics. */
#define FILTER_DIMENSION 6

/** \brief Link length */
#define LINK_LENGTH 0.295
/** \brief Link mass */
#define LINK_MASS 0.125
/** \brief Inertia at centre of mass */
#define INERTIA_AT_COM (1.0/12.0)*LINK_MASS*pow(LINK_LENGTH,2)

/** \brief Mass of magnet */
#define MAGNET_MASS 0.075

/** \brief Gear ratio of damper motor (i.e., of motor gear head). */
#define DAMPER_MOTOR_GEAR_RATIO 19
/** \brief Motor inertia of damper motor. */
#define DAMPER_MOTOR_INERTIA 41.8e-7
/** \brief Inertia of damper motor due to gears. */
#define DAMPER_MOTOR_GEAR_INERTIA 0.4e-7

/** \brief Total inertia of link. */
#define INERTIA .525*(INERTIA_AT_COM+LINK_MASS*pow(LINK_LENGTH/2.0,2)+pow(DAMPER_MOTOR_GEAR_RATIO,2)*(DAMPER_MOTOR_INERTIA+DAMPER_MOTOR_GEAR_INERTIA)+MAGNET_MASS*pow(LINK_LENGTH,2))
/* #define INERTIA INERTIA_AT_COM+LINK_MASS*pow(LINK_LENGTH/2.0,2)+pow(DAMPER_MOTOR_GEAR_RATIO,2)*(DAMPER_MOTOR_INERTIA+DAMPER_MOTOR_GEAR_INERTIA)+MAGNET_MASS*pow(LINK_LENGTH,2)
 */

/** 
 * \brief Spring constant of spring.
 * 
 * \note This is taken from the spring data sheet (see
 * doc/Z-081K-01I-datasheet.ps)
 */
#define SPRING_CONSTANT 323

/** \brief Length of lever attached to equilibrium position servo. */
#define LEVER_LENGTH 0.03

#define PIN_DISPLACEMENT_NEAR   0.16
#define PIN_DISPLACEMENT_MEDIUM 0.17
#define PIN_DISPLACEMENT_FAR    0.18
/** \brief Distance from base pin to joint axis. */
#define PIN_DISPLACEMENT PIN_DISPLACEMENT_FAR

#define SMALL_DRUM_RADIUS 0.0075 /* small drum */
#define LARGE_DRUM_RADIUS 0.01   /* large drum */
/** \brief Radius of drum attached to stiffness servo (metres). */
#define DRUM_RADIUS LARGE_DRUM_RADIUS

/** Alex: select how thread is wound in drum, to change direction of rotation. 
 * Default is clockwise winding, 
 * if DRUM_WINDING_COUNTERCLOCKWISE is defined, it will wind up counterclockwise 
 */
#define DRUM_WINDING_COUNTERCLOCKWISE


/** \brief Viscous friction coefficient.
 * \note Currently, this value must be tuned by hand after running the system-ID script. */
#define VISCOUS_FRICTION (2.5e-3)-(1.4529e-04)
/* #define VISCOUS_FRICTION 0.0
 */
/* #define VISCOUS_FRICTION 2*5*0.004517508630
 */

/** \brief Viscous friction coefficient.
 * \note Currently, this value must be tuned by hand after running the system-ID script. */
#define COULOMB_FRICTION 0.0

/** 
 * \brief Constant for calculating joint damping torque.
 */
/* #define DAMPING_CONSTANT 2*5*0.004517508630
 */
#define DAMPING_CONSTANT 0.001

/**
 * \brief Constant for calculating joint torque due to gravity.
 */
#define GRAVITY_CONSTANT 0

/** \brief Resistance of R_sense resistor (Ohms) in current sensing circuit. */
#define CURRENT_RSENSE 0.1

/** \brief Resistance of R1 resistor (Ohms) in current sensing circuit. */
#define CURRENT_R1 4200.0

/** \brief Resistance of R2 resistor (Ohms) in current sensing circuit. */
#define CURRENT_R2 21800.0

#endif

