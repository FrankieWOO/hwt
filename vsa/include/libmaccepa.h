/** 
 * \file libmaccepa.h 
 * \brief Library of functions for calculating dynamics properties of the MACCEPA.
 * \ingroup MACCEPA
 */
#ifndef __libmaccepa_h
#define __libmaccepa_h
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "../sketchbook/maccepa/defines.h"

/** \brief State dimensionality. */
#define DIMX 2*DIMQ

/** \brief struct containing dynamics model parameters. */
typedef struct {
	/** \brief Inertia of free link.          */
	double inertia; 
	/** \brief Free link length.          */
	double link_length; 
	/** \brief Spring constant of spring (assuming linear spring).          */
	double spring_constant;
	/** \brief Length of lever that sets eq. position (attached to servo0). */
	double lever_length;   
	/** \brief Distance of pin from joint rotation axis.                    */
	double pin_displacement; 
	/** \brief Radius of drum attached to stiffness servo (servo1).         */
	double drum_radius;
	/** \brief Constant for calculating damping torque as a function of joint anglular velocity. 
     *
     *  This is calculated as \f$ \tau(q) = b_c \dot{q} \f$ where \f$ b_c \f$ is this constant.
     */
	double damping_constant;
	/** \brief Constant for calculating gravity force as a function of joint angle. 
     *
     *  This is calculated as \f$ g(q) = g_c sin(q) \f$ where \f$ g_c \f$ is this constant.
     *  
	 *  \note If the maccepa is mounted horizontally, this should be set to zero.
     */
	double gravity_constant;
	/** \brief Constant for calculating viscous friction. 
     */
	double viscous_friction;
	/** \brief Constant for calculating coulomb friction. 
     */
	double coulomb_friction;
	/** \brief Maximum command (in radiens). 
     */
	double umax[DIMU];
	/** \brief Minumum command (in radiens). 
     */
	double umin[DIMU];

	/** \brief Vector of FIR filter paramters.                              */
	double b_filter[2*FILTER_DIMENSION];      
} maccepa_model;

void maccepa_model_init                              ( maccepa_model * model);
void maccepa_model_get_torque                        ( double * tau, double * x, double * u, maccepa_model * model );
void maccepa_model_get_actuator_torque               ( double * tau, double * x, double * u, maccepa_model * model );
void maccepa_model_get_damping_torque                ( double * tau, double * x, double * u, maccepa_model * model );
void maccepa_model_get_gravity_torque                ( double * tau, double * x, double * u, maccepa_model * model );
void maccepa_model_get_friction_torque               ( double * tau, double * x, double * u, maccepa_model * model );
void maccepa_model_get_acceleration                  ( double * acc, double * x, double * u, maccepa_model * model );
void maccepa_model_get_equilibrium_position          ( double *  q0, double * x, double * u, maccepa_model * model );
void maccepa_model_get_equilibrium_position_jacobian ( double *   J, double * x, double * u, maccepa_model * model );
void maccepa_model_get_stiffness                     ( double *   k, double * x, double * u, maccepa_model * model );
void maccepa_model_get_stiffness_jacobian            ( double *   J, double * x, double * u, maccepa_model * model );
void maccepa_model_get_damping                       ( double *   b, double * x, double * u, maccepa_model * model );
void maccepa_model_get_spring_force                  ( double *   f, double * x, double * u, maccepa_model * model );
void maccepa_model_get_motor_positions               ( double *   m, double * x, maccepa_model * model             );

#endif

