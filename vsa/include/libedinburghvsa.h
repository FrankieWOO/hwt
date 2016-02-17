/** 
 * \file libedinburghvsa.h 
 * \brief Library of functions for calculating dynamics properties of the Edinburgh VSA.
 * \ingroup edinburghvsa
 */
#ifndef __libedinburghvsa_h
#define __libedinburghvsa_h
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "../sketchbook/edinburghvsa/defines.h"

/** \brief Dimensionality of joint space. */
#define DIMQ 1
/** \brief State dimensionality. */
#define DIMX 2*DIMQ
/** \brief Action dimensionality. */
#define DIMU 2

/** \brief struct containing dynamics model parameters. */
typedef struct {
	/** \brief Inertia of free link.          */
	double inertia; 
	/** \brief Distance between arm axis and mounting point of springs */
	double link_lever_length;
	/** \brief Spring constant of springs (assuming two springs are identical).          */
	double spring_constant;
	/** \brief Length of servo levers, i.e., distance between motor axis and mounting point of springs */
	double lever_length;   
	/** \brief Distance between center between motor axes and arm axis   */
	double joint_to_motor_axis_y_separation;
	/** \brief Distance between center and motor axes */
	double joint_to_motor_axis_x_separation;
	/** \brief Spring rest length */
	double spring_rest_length;
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
} edinburghvsa_model;

void edinburghvsa_model_init                              ( edinburghvsa_model * model);
void edinburghvsa_model_get_torque                        ( double * tau, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_actuator_torque               ( double * tau, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_damping_torque                ( double * tau, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_gravity_torque                ( double * tau, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_friction_torque               ( double * tau, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_acceleration                  ( double * acc, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_equilibrium_position          ( double *  q0, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_equilibrium_position_jacobian ( double *   J, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_stiffness                     ( double *   k, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_stiffness_jacobian            ( double *   J, double * x, double * u, edinburghvsa_model * model );
void edinburghvsa_model_get_motor_positions               ( double *   m, double * x, edinburghvsa_model * model             );

#endif

