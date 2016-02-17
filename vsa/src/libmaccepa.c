/** \defgroup MACCEPA
 */
/** \page maccepa_information
 *
 * \section maccepa_info Information about the MACCEPA implementation
 *
 * \image html maccepa_hardware.png "MACCEPA"
 *
 * \subsection dynamics Dynamics
 *
 * The MACCEPA dynamics library contains functions for calculating dynamics
 * properties of the robot based on an analytical model of the robot. The
 * geometry of this model is shown below: \image html maccepa_model.png
 *
 * The dynamics equations are as follows.
 *
 * \li Joint torque: \f$\tau(q,\mathbf{u}) = \kappa
 * BC\sin(u_1+q)\left(1+\frac{ru_2-(C-B)}{\sqrt{B^2 + C^2 -
 * 2BC\cos(u_1+q)}}\right)\f$
 *
 * where \f$B\f$, \f$C\f$ are shown in the diagram above, \f$r\f$ is the radius
 * of the drum attached to the pretensioning servo, and \f$\kappa\f$ is the
 * spring constant of the spring.
 *
 * \li Joint stiffness: \f$\begin{array}{rl}k(\mathbf{x},\mathbf{u}) =&\kappa
 * BC\cos(u_1+q)\left( 1 + \frac{ru_2 - (C-B)}{\sqrt{B^2+C^2-2BC\cos(u_1+q)}}
 * \right)\\ &-\kappa BC\sin(u_1+q)\frac{(ru_2-(C-B))BC\sin(u_1+q)}{(B^2 + C^2
 * - 2BC\cos(u_1+q))^{\frac{3}{2}}}\end{array}\f$
 *
 * \li Equilibrium Position: \f$q_{0}=u_1\f$
 *
 * \sa The maxima script doc/maxima/maccepa.mac for derivations.
 *
 * The dynamics parameters (inertia, spring constants, lengths) are measured by
 * hand or estimated with a system identification. These are defined in \ref
 * sketchbook/maccepa/defines.h
 *
 * \subsection sys_id System Identification
 * 
 * All estimated parameters from system identification are stored in
 * sketchbook/maccepa/defines.h
 *
 * The process of system identifiaction is data driven, based on least squares
 * fitting, and (mostly) automated through matlab scripts. The top level script
 * for this is <a
 * href="../m2html/MACCEPA/identify_maccepa.html">identify_maccepa.m</a>.  This
 * is an interactive script, and will print out instructions as it runs (see
 * the matlab documentation for details).  It basically calls successive
 * system-id scripts for fitting different parameters in sequence, starting with
 * the servo pots, onto the joint sensors, then onto the dynamics parameters
 * such as inertia. As there is interdependency in the parameters (i.e., the
 * estimation of some parameters depends on - and is bootstrapped
 * from - estimates of other parameters), the script requires that you
 * update these in sketchbook/maccepa/defines.h at each stage of the
 * identification.  After each stage, the script recompiles the underlying c
 * code to incorporate the new estimates. Note that after system identifcation
 * is complete, you can re-run the identification script to check that the
 * parameters have been properly updated (e.g., many of the sensor 'scaling'
 * parameters will be approximately 1, and the 'offset' parameters
 * approximately 0).
 *
 * \subsection system_characterisation System Characterisation
 *
 * Based on this model of the dynamics, the below figures give some
 * characterisation of the system dynamics.
 * 
 * \image html eqpos_vs_u_maccepa.png "Equlibrium position vs. motor positions (evaluated at q=0, dq/dt=0)" 
 *
 * \image html stiff_vs_u_maccepa.png "Stiffness vs. motor positions (evaluated at q=0, dq/dt=0)"
 *
 * \subsection figures Useful facts and figures:
 *
 * \li The maximum range of the joint is \f$-\frac{\pi}{2}\le q
 * \le\frac{\pi}{2}\f$ radiens.
 *
 * \li The maximum range of commands to servo 0 (equilibrium position servo) is
 * \f$-\frac{\pi}{2}\le u_1\le\frac{\pi}{2}\f$ radiens.
 *
 * \li The maximum range of commands to servo 1 (stiffness servo) is \f$0\le
 * u_2\le\pi\f$ radiens.
 *
 * \li The maximum stiffness range  is \f$-0.657894\le k\le 0.415422\f$ Nm/rad.
 * \note This stiffness range is based on the gradient of the torque function
 * evaluated away from the equilibrium. For this reason, negative values are
 * possible, however this is not neccessarily an indicator of instability!
 * (For this, consider the sign of the torque at a given point).  This range
 * was estimated by sampling the gradient of the torque function, over the full
 * range of the joint angle and servo positions.
 *
 * \li The maximum range of equilibrium positions is \f$-\frac{\pi}{2}\le
 * q_0\le\frac{\pi}{2}\f$ radiens.
 * 
 * \subsection components Components
 * Datasheets for the componenets used can be found below.
 * \li <a href="../datasheets/doc8161.pdf">Microcontroller (Atmel 328)</a>
 * \li <a href="../datasheets/HSR5990TG.pdf">Servos (Hitec HSR-5990TG)</a>
 * \li <a href="../datasheets/MLX90316.pdf">Joint angle sensor (MLX90316 Rotary Position Sensor)</a>
 * \li <a href="../datasheets/Z-081K-01I-datasheet.pdf">Spring</a>
 * \li <a href="../datasheets/A-max-22-110117_10_EN_097.pdf">Damper motor (Maxon A-max 22/110125)</a> and <a href="../datasheets/GP-22-L-232763_10_EN_221.pdf">gearhead (Maxon GP 22 L/232768)</a>
 * \li <a href="../datasheets/LM358.pdf">Op-Amp for current sensing circuit</a>
 * \li <a href="../datasheets/L293D.pdf">H-bridge (for controlling the electro-magnet)</a>
 *
 * \subsection current_sensor Current Sensing Circuit
 * Current sensing circuit is show below. 
 * \image html current_sensor.png
 * Current can be calculated as \f$ V_{out} = I R_s ( 1 + R_2/R_2) \to I= \frac{V_{out}}{R_s(1+R_2/R_2)}\f$.
 * \sa Forum discussions <a href="http://letsmakerobots.com/node/10960">here</a> and <a href="http://letsmakerobots.com/node/18517">here</a> 
 *
 *
 *
 * \subsection references References
 *
 * The MACCEPA design was originally proposed in: 
 *
 * \li Ham, R. V.; Vanderborght, B.;
 * Damme, M. V.; Verrelst, B. & Lefeber, D.  MACCEPA, the mechanically
 * adjustable compliance and controllable equilibrium position actuator: Design
 * and implementation in a biped robot Robotics and Autonomous Systems, 2007,
 * 55, 761-768
 *
 * \ingroup MACCEPA
 */

/** 
 * \file libmaccepa.c 
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \date August 2010
 * \brief Library of functions for calculating dynamics properties of the MACCEPA.
 * \ingroup MACCEPA
 * \sa <a href="../m2html/MACCEPA/m-files/dynamics/index.html">Matlab wrappers to this library</a>
 */
#include <libmaccepa.h>

/** \brief Initialise model struct (set default parameters).
 *  \param model model struct.
 */
void maccepa_model_init(maccepa_model *model) {
	model->inertia          = INERTIA;
	model->link_length      = LINK_LENGTH;
   	model->spring_constant  = SPRING_CONSTANT;
	model->lever_length     = LEVER_LENGTH;
	model->pin_displacement = PIN_DISPLACEMENT;
	model->drum_radius      = DRUM_RADIUS;
    model->damping_constant = DAMPING_CONSTANT;
    model->gravity_constant = GRAVITY_CONSTANT;
    model->viscous_friction = VISCOUS_FRICTION;
    model->coulomb_friction = COULOMB_FRICTION;
    model->umax[0]          = U_ULIM_RAD_SERVO0;
    model->umax[1]          = U_ULIM_RAD_SERVO1;
#ifdef  VARIABLE_DAMPING
    model->umax[2]          = U_ULIM_DAMPER0;
#endif     /* -----  not VARIABLE_DAMPING  ----- */
    model->umin[0]          = U_LLIM_RAD_SERVO0;
    model->umin[1]          = U_LLIM_RAD_SERVO1;
#ifdef  VARIABLE_DAMPING
    model->umin[2]          = U_LLIM_DAMPER0;
#endif     /* -----  not VARIABLE_DAMPING  ----- */
    model->b_filter[ 0]     = SERVO0_FIR_B0;
    model->b_filter[ 1]     = SERVO0_FIR_B1;
    model->b_filter[ 2]     = SERVO0_FIR_B2;
    model->b_filter[ 3]     = SERVO0_FIR_B3;
    model->b_filter[ 4]     = SERVO0_FIR_B4;
    model->b_filter[ 5]     = SERVO0_FIR_B5;
    model->b_filter[ 6]     = SERVO1_FIR_B0;
    model->b_filter[ 7]     = SERVO1_FIR_B1;
    model->b_filter[ 8]     = SERVO1_FIR_B2;
    model->b_filter[ 9]     = SERVO1_FIR_B3;
    model->b_filter[10]     = SERVO1_FIR_B4;
    model->b_filter[11]     = SERVO1_FIR_B5;
}

/** \brief Calculate joint accleration as a function of current state and command. 
 *  \param[out] acc joint acceleration
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$ \ddot{q}(\mathbf{x},\mathbf{u}) = (\tau_{actuators} - \tau_{damping} - \tau_{gravity} - \tau_{friction})/I \f$
 *
 */
void maccepa_model_get_acceleration ( double * acc, double * x, double * u, maccepa_model * model ) {

	double I = model->inertia;
	double tau;
 	maccepa_model_get_torque( &tau, x, u, model );
 	acc[0] = tau/I;

	return;
}

/** 
 *  \brief Calculate total joint torque as a function of current state and command. 
 *  \param[out] tau torque
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$ \tau(\mathbf{x},\mathbf{u}) = \tau_{actuators} - \tau_{damping} - \tau_{gravity} - \tau_{friction} \f$
 *
 */
void maccepa_model_get_torque( double * tau, double * x, double * u, maccepa_model * model ) {

	double tau_actuator;  maccepa_model_get_actuator_torque ( &tau_actuator, x, u, model );
	double tau_damping ;  maccepa_model_get_damping_torque  ( &tau_damping , x, u, model );
	double tau_gravity ;  maccepa_model_get_gravity_torque  ( &tau_gravity , x, u, model );
	double tau_friction;  maccepa_model_get_friction_torque ( &tau_friction, x, u, model );

	tau[0] = tau_actuator - tau_damping - tau_gravity - tau_friction;

	return;
}

/** \brief Calculate joint torque due to actuators as a function of current state and command. 
 *  \param[out] tau torque
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements 
 *  \f$ \tau(q,\mathbf{u}) = \kappa BC\sin(u_1-q)\left(1+\frac{ru_2-(C-B)}{\sqrt{B^2 + C^2 - 2BC\cos(u_1-q)}}\right)\f$
 *
 * \note This calculation is based on the derivation given in:
 *   Ham, R. V.; Vanderborght, B.; Damme, M. V.; Verrelst, B. & Lefeber, D.
 *   MACCEPA, the mechanically adjustable compliance and controllable
 *   equilibrium position actuator: Design and implementation in a biped
 *   robot, Robotics and Autonomous Systems, 2007, 55, 761-768
 *
 */
void maccepa_model_get_actuator_torque( double * tau, double * x, double * u, maccepa_model * model ) {

	double k = model->spring_constant;
	double B = model->lever_length;
	double C = model->pin_displacement;
	double r = model->drum_radius;
	double a = u[0]-x[0];

 	tau[0] = k*B*C*sin(a)*( 1 + (r*u[1]-(C-B) )/sqrt(pow(B,2)+pow(C,2)-2*B*C*cos(a)));

	return;
}

/** \brief Calculate joint torque due to damping as a function of current state and command. 
 *  \param[out] tau torque
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$ \tau(\mathbf{x},\mathbf{u}) = b_c\dot{q} \f$
 *
 *  \note Currently, if VARIABLE_DAMPING is defined, this returns zero torque.
 *  Temporarily, the damping modelling is being done on the Matlab side until
 *  we get a good damping model (or until we know the right model to use).
 *
 */
void maccepa_model_get_damping_torque( double * tau, double * x, double * u, maccepa_model * model ) {

	double b; maccepa_model_get_damping( &b, x, u, model );
  	tau[0] = b*x[1]; 

	return;
}

/** \brief Calculate joint torque due to gravity as a function of current state and command. 
 *  \param[out] tau torque
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$ \tau(\mathbf{x},\mathbf{u}) = g_c\sin{q} \f$
 *
 */
void maccepa_model_get_gravity_torque( double * tau, double * x, double * u, maccepa_model * model ) {

	double gc= model->gravity_constant;
 	tau[0] = gc*sin(x[0]);

	return;
}

/** \brief Calculate joint torque due to friction as a function of current state and command. 
 *  \param[out] tau torque
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$ \tau(\mathbf{x},\mathbf{u}) = f_v \dot{q} + f_c sign(\dot{q}) \f$ where \f$ f_v \f$ and \f$ f_c \f$ are the viscous and Coulomb friction constants, respectively.
 *
 */
void maccepa_model_get_friction_torque( double * tau, double * x, double * u, maccepa_model * model ) {

 	tau[0] = model->viscous_friction*x[1] + model->coulomb_friction*copysign(1.0,x[1]);

	return;
}


/** \brief Calculate stiffness as a function of current state and command.
 *  \param[out] k joint stiffness
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements 
 *  \f$k(q,\mathbf{u})=-\frac{\partial\tau}{\partial q}|_{q}= \kappa BC\cos(u_1-q)\left(1+\frac{(ru_2-(C-B)}{\sqrt{B^2+C^2-2BC\cos(u_1-q)}}\right) -\frac{\kappa B^2 C^2\sin^2(u_1-q)(ru_2-(C-B))}{(B^2+C^2-2BC\cos(u_1-q))^{\frac{3}{2}}}\f$ 
 *
 *  where \f$\tau\f$ is the actuator torque (i.e., torque due to 
 *  the variable stiffness mechanism).
 */
void maccepa_model_get_stiffness ( double * k, double * x, double * u, maccepa_model * model ) {
	
	double B     = model->lever_length;
	double C     = model->pin_displacement;
	double kappa = model->spring_constant;
	double r     = model->drum_radius;   
	double a     = u[0]-x[0];
	double L     = sqrt(pow(B,2)+pow(C,2)-2*B*C*cos(a));
	double b     = r*u[1]-(C-B);

	k[0]         = kappa*B*C*cos(a)*(1+b/L)
                  -kappa*(pow(B*C*sin(a),2))*b/pow(L,3);

	return;
}     

/** \brief Calculate Jacobian of stiffness with respect to motor commands (as a function of current state and command).
 *  \param[out] J joint stiffness Jacobian
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  \todo This calculation is currently based on finite differences. Accuracy
 *  and effieciency could be improved by finding the analytic expression for
 *  this.
 */
void maccepa_model_get_stiffness_jacobian ( double * J, double * x, double * u, maccepa_model * model ) {
	int i;
	double delta=1e-6;
	double kp,km,ud[DIMU];

	for ( i = 0; i < DIMU; i += 1 ) {
		memcpy(ud,u,DIMU*sizeof(double)); ud[i]+=delta; maccepa_model_get_stiffness(&kp,x,ud,model);
		memcpy(ud,u,DIMU*sizeof(double)); ud[i]-=delta; maccepa_model_get_stiffness(&km,x,ud,model);
		J[i] = (kp-km)/(2*delta);
	}

	return;
}     

/** \brief Calculate equilibrium position as a function of current state and command.
 *  \param[out] q0 joint equilibrium position
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$q_{0}=u_1\f$
 */
void maccepa_model_get_equilibrium_position ( double * q0, double * x, double * u, maccepa_model * model ) {

 	q0[0] = u[0];

	return;
}     

/** \brief Calculate Jacobian of equilibrium position with respect to motor commands (as a function of current state and command).
 *  \param[out] J joint equilibrium position
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$\mathbf{J}_{q_0}=(1,0,0)^\top\f$
 */
void maccepa_model_get_equilibrium_position_jacobian ( double * J, double * x, double * u, maccepa_model * model ) {

	J[0] = 1;
	J[1] = 0;
#ifdef  VARIABLE_DAMPING
	J[2] = 0;
#endif     /* -----  not VARIABLE_DAMPING  ----- */

	return;
}

/** \brief Calculate damping as a function of current state and command.
 *  \param[out] b joint damping
 *  \param[in]  x state (position, velocity)
 *  \param[in]  u command (motor positions (rad), damping command (duty cycle))
 *  \param[in]  model model struct
 *
 *  \note This is a place holder for a damping model!
 *
 *  \todo Integrate model of damper motor here.
 */
	void
maccepa_model_get_damping ( double * b, double * x, double * u, maccepa_model * model )
{
/* 
 * 	int i;
 * 	int    Nc     = model.Nc_damping_model;
 * 	double s2     = model.s2_damping_model;
 * 	double w[Nc]; memcpy(w,model.w_damping_model,Nc*sizeof(double));
 *    	double c[Nc]; memcpy(c,model.c_damping_model,Nc*sizeof(double));
 * 	double d[Nc],phi[Nc],sumphi=0;
 * 	for ( i = 0; i < Nc; i += 1 ) {
 * 		d  [i] = u[2]-c[i];
 * 		phi[i] = exp(-(0.5/s2)*d[i]); 
 * 		sumphi+=phi[i];
 * 	}
 * 	b[0]=0;
 * 	for ( i = 0; i < Nc; i += 1 ) {
 * 		phi[i]/=sumphi;
 * 		b[0] += w[i]*phi[i];
 * 	}
 */
#ifdef  VARIABLE_DAMPING
  	b[0]=0;
#else      /* -----  not VARIABLE_DAMPING  ----- */
	b[0]=model->damping_constant;
#endif     /* -----  not VARIABLE_DAMPING  ----- */

	return ;
}		/* -----  end of function maccepa_model_get_damping  ----- */

/** \brief Calculate spring force as a function of current state and command. 
 *  \param[out] force spring force
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$ F = \kappa (l-l_0) \f$ where \f$ l-l_0 \f$ is the spring extension.
 *
 */
void maccepa_model_get_spring_force( double * f, double * x, double * u, maccepa_model * model ) {

	double k = model->spring_constant;
	double B = model->lever_length;
	double C = model->pin_displacement;
	double r = model->drum_radius;
	double a = u[0]-x[0];
	double L0 = C-B;
	double L = sqrt(pow(B,2)+pow(C,2)-2*B*C*cos(a)) + r*u[1];

	f[0] = -k*(L-L0);

	return;
}

/** 
 * \brief Calculate motor positions based on finite impulse response filters. 
 * \param[out] m motor positions
 * \param[in]  x filter states (velocity, acceleration)
 * \param[in]  model model struct
 * \todo This function is no longer used and should be removed.
 */
void maccepa_model_get_motor_positions ( double * m, double * x, maccepa_model * model ) {

	int i;

	m[0]=0;
	m[1]=0;
	for ( i = 0; i < FILTER_DIMENSION; i += 1 ) {
		m[0]+=model->b_filter[i]*x[i];
		m[1]+=model->b_filter[i+FILTER_DIMENSION]*x[i+FILTER_DIMENSION];
	}

	return;
}

