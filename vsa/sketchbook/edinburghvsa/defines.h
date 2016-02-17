/** 
 * \file sketchbook/edinburghvsa/defines.h
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \ingroup Swinger
 *
 * \brief Defines for edinburghvsa.pde.
 */
#ifndef __edinburghvsa_defines_h
#define __edinburghvsa_defines_h

/* \brief Dimensionality of command vector (number of servos). */
#define DIMU 2

/* \brief Dimensionality of observation vector (number of sensors). */
#define DIMY 4

/** \brief Baudrate used for serial communication. */
#define BAUDRATE 38400
/** \brief No. of bytes in 'transmit' bit-string. */
#define TRANSMIT_LENGTH 10
/** \brief First header byte of 'transmit' bit-string. */
#define TRANSMIT_HEADER0 0xFF
/** \brief Second header byte of 'transmit' bit-string. */
#define TRANSMIT_HEADER1 0xFF
/** \brief No. of bytes in 'receive' bit-string. */
#define RECEIVE_LENGTH 4
/** \brief Header byte of 'receive' bit-string. */
#define RECEIVE_HEADER 0xFF

/** \brief PWM output pin on Arduino for controlling Servo 0. */
#define OUTPUT_PIN_SERVO0 9
/** \brief PWM output pin on Arduino for controlling Servo 1. */
#define OUTPUT_PIN_SERVO1 10

/** \brief First analogue input pin on Arduino for reading joint position sensor (potentiometer). */
#define INPUT_PIN_JOINT_POT0 0
/** \brief Second analogue input pin on Arduino for reading joint position sensor (potentiometer). */
#define INPUT_PIN_JOINT_POT1 1
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
 */
#define U_MIN_SERVO0 1120
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
#define U_MIN_SERVO1 1440

/**
 * \brief Maximum commanded position to Servo 0 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 180 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has maximum
 * angle 2400 microseconds (so set this to 4800).
 */
#define U_MAX_SERVO0 4416
/**
 * \brief Maximum commanded position to Servo 1 (in
 * units of 0.5 microseconds).
 *
 * \note This should correspond to 180 degrees and may
 * need to be adjusted according to the actual servo.
 * According to the manual, the HSR-5990TG has maximum
 * angle 2400 microseconds (so this should be around 4800).
 */
#define U_MAX_SERVO1 4736

/**
 * \brief Initial position command for Servo 0 (in
 * units of 0.5 microseconds).
 */
#define U_INIT_SERVO0 U_MAX_SERVO0
/**
 * \brief Initial position command for Servo 1 (in
 * units of 0.5 microseconds).
 */
#define U_INIT_SERVO1 U_MIN_SERVO1

/** \brief Maximum commanded position to Servo 0 in units of radiens. */
#define U_MAX_RAD_SERVO0 M_PI
/** \brief Maximum commanded position to Servo 1 in units of radiens. */
#define U_MAX_RAD_SERVO1 M_PI

/** \brief Minimum commanded position to Servo 0 in units of radiens. */
#define U_MIN_RAD_SERVO0 0
/** \brief Minimum commanded position to Servo 1 in units of radiens. */
#define U_MIN_RAD_SERVO1 0

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
#define U_ULIM_RAD_SERVO1 U_MAX_RAD_SERVO1

/** 
 * \brief Lower command limit for Servo 0 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * minimum position that the servo can reach (as
 * defined by U_MIN_SERVO0). 
 */
#define U_LLIM_RAD_SERVO0 U_MIN_RAD_SERVO1
/** 
 * \brief Lower command limit for Servo 1 (in
 * radiens).
 *
 * \note This may not necessarily correspond to the
 * minimum position that the servo can reach (as
 * defined by U_MIN_SERVO1). 
 */
#define U_LLIM_RAD_SERVO1 U_MIN_RAD_SERVO1

/**
 * \brief Scaling factor for converting raw joint potentiometer readings to angles (radiens).
 *
 * \todo System-ID for this. Currently this is hacked based on Djordje and Stefan's code.
 */
#define W_POT_JOINT 0.055*M_PI/180.0
/** 
 * \brief Scaling factor for converting raw joint
 * accelerometer readings to anglular accleration
 * (radiens per second squared).
 */
#define W_ACC_JOINT 0.15
/**
 * \brief Scaling factor for converting raw pot
 * readings from Servo 0 to the motor angle (in
 * radiens). 
 */
#define W_POT_SERVO0 0.00150
/**
 * \brief Scaling factor for converting raw pot
 * readings from Servo 1 to the motor angle (in
 * radiens). 
 *
 * \todo System-ID for this. Currently this is hacked
 * based on Djordje and Stefan's code.
 */
#define W_POT_SERVO1 -0.001540

/**
 * \brief Offset factor for converting raw joint
 * potentiometer readings to angle (radiens).
 *
 * \todo System-ID for this. Currently this is hacked
 * based on Djordje and Stefan's code.
 */
#define C_POT_JOINT -0.11-3778*0.055*M_PI/180
/**
 * \brief Offset factor for converting raw joint
 * accelerometer readings to anglular acceleration
 * (radiens per second squared).
 *
 * \todo System-ID for this. Currently this is hacked
 * based on Djordje and Stefan's code.
 */
#define C_ACC_JOINT -2070*0.15-0.5
/**
 * \brief Offset factor for converting raw pot
 * readings from Servo 0 to the motor angle (in
 * radiens). 
 *
 * \todo System-ID for this. Currently this is hacked
 * based on Djordje and Stefan's code.
 */
#define C_POT_SERVO0 -0.62-0.016
/**
 * \brief Offset factor for converting raw pot
 * readings from Servo 1 to the motor angle (in
 * radiens). 
 *
 * \todo System-ID for this. Currently this is hacked
 * based on Djordje and Stefan's code.
 */
#define C_POT_SERVO1 3.58+0.105

/** \brief FIR filter paramters for servo 0. */
#define SERVO0_FIR_B0 0
#define SERVO0_FIR_B1 0
#define SERVO0_FIR_B2 0
#define SERVO0_FIR_B3 0
#define SERVO0_FIR_B4 0
#define SERVO0_FIR_B5 0

/** \brief FIR filter paramters for servo 1. */
#define SERVO1_FIR_B0 0
#define SERVO1_FIR_B1 0
#define SERVO1_FIR_B2 0
#define SERVO1_FIR_B3 0
#define SERVO1_FIR_B4 0
#define SERVO1_FIR_B5 0

/** \brief Dimensionality of FIR filters for modelling servo motor dynamics. */
#define FILTER_DIMENSION 6

/** \brief Total inertia of link. */
#define INERTIA 0.0011
/** \brief Length of lever (i.e., distance from servo rotation axis to spring attachment point). */
#define LEVER_LENGTH 0.026
/** \brief Spring constant (assumed equal for both springs). */
#define SPRING_CONSTANT 424
/** \brief Spring rest length (assumed equal for both springs). */
#define SPRING_REST_LENGTH 0.02737
/** \brief Distance from motor axes to joint axis in x direction - 'h' in diagram. */
#define JOINT_TO_MOTOR_AXIS_X_SEPARATION 0.027
/** \brief Distance from motor axes to joint axis in y direction - 'd' in diagram. */
#define JOINT_TO_MOTOR_AXIS_Y_SEPARATION 0.081
/** \brief Length of lever on free link (i.e., distance from link axis to spring attachment point) - 'a' in diagram. */
#define LINK_LEVER_LENGTH 0.026
/** \brief Viscous friction coefficient.
 * \note Currently, this value must be tuned by hand after running the system-ID script. */
#define VISCOUS_FRICTION 0.0050


/** \brief Viscous friction coefficient.
 * \note Currently, this value must be tuned by hand after running the system-ID script. */
#define COULOMB_FRICTION 0

/** 
 * \brief Constant for calculating joint damping torque.
 */
#define DAMPING_CONSTANT 0

/**
 * \brief Constant for calculating joint torque due to gravity.
 */
#define GRAVITY_CONSTANT 0

#endif

