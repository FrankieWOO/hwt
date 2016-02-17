/** \defgroup edinburghvsa Edinburgh VSA
 */
/** \page edinburghvsa_information
 *
 * \section edinburghvsa_info Information about the Edinburgh VSA implementation
 *
 * \image html edinburghvsa_hardware.png "Edinburgh VSA"
 *
 * \subsection dynamics Dynamics
 *
 * The Edinburgh VSA dynamics library contains functions for calculating dynamics
 * properties of the robot based on an analytical model of the robot. The
 * geometry of this model is shown below: \image html edinburghvsa_model.png
 *
 * The dynamics equations are as follows.
 *
 * \li Joint torque: \f$\tau(q,\mathbf{u}) = \hat{\mathbf z}^T(\mathbf F_1(q,\mathbf{u})\times\mathbf a_1(q) + \mathbf F_2(q,\mathbf{u})\times \mathbf a_2(q))\f$
 *
 * where \f$\hat{\mathbf z}\f$ is the unit vector along the joint rotation
 * axis, \f$\mathbf{a}=(a\cos q,a\sin q,0)^T\f$, \f$\mathbf F_i=\kappa(s_i -
 * s_0)\frac{\mathbf s_i}{s_i}\f$, \f$i\in\{1,2\}\f$ are the forces due to the
 * two springs (both with spring constant \f$\kappa\f$),
 * \f$\mathbf{s}_1=(-h-L\sin u_1,-d+L\cos u_1,0)^T+\mathbf{a}\f$ and
 * \f$\mathbf{s}_2=(h+L\sin u_2,-d+L\cos u_2,0)^T-\mathbf{a}\f$ are the
 * extensions of the two springs, and all other quantities are illustrated in
 * the figure above.
 *
 *
 * The dynamics parameters (inertia, spring constants, lengths) are measured by
 * hand or estimated with a system identification. These are defined in \ref
 * sketchbook/edinburghvsa/defines.h
 *
 * \sa The maxima script doc/maxima/edinburghvsa.mac for derivations.
 *
 * \subsection system_characterisation System Characterisation
 *
 * Based on this model of the dynamics, the below figures give some
 * characterisation of the system dynamics.
 * 
 * \image html eqpos_vs_u_edinburghvsa.png "Equlibrium position vs. motor positions (evaluated at q=0, dq/dt=0)" 
 *
 * \image html stiff_vs_u_edinburghvsa.png "Stiffness vs. motor positions (evaluated at q=0, dq/dt=0)"
 *
 * \subsection figures Useful facts and figures:
 *
 * \li The maximum range of the joint is (approximately) \f$-65^\circ\le q \le
 * 65^\circ\f$.
 *
 * \li The maximum range of commands to servo 0 is \f$0\le u_1\le\pi\f$
 * radiens.
 *
 * \li The maximum range of commands to servo 1 is \f$0\le u_2\le\pi\f$
 * radiens.
 *
 * \li The maximum stiffness range is \f$ -0.065992 \le k \le 0.514579\f$
 * Nm/rad.  
 *
 * \note This stiffness range is based on the gradient of the torque
 * function evaluated away from the equilibrium. For this reason, negative
 * values are possible, however this is not neccessarily an indicator of
 * instability! (For this, consider the sign of the torque at a given point).
 * This range was estimated by sampling the gradient of the torque function,
 * over the full range of the joint angle and servo positions.
 *
 * \li The maximum range of equilibrium positions is (approximately)
 * \f$-25^\circ\le q\le 25^\circ\f$.  \note This stiffness range is based on
 * finding the zeros of the torque function over the range of servo positions
 * and feasible joint angles.
 *
 * \subsection components Components
 * Datasheets for the componenets used can be found below.
 * \li <a href="../datasheets/doc8161.pdf">Atmel 328 Microcontroller</a>
 *
 * \ingroup edinburghvsa
 */

/** 
 * \file libedinburghvsa.c 
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \date August 2010
 * \brief Library of functions for calculating dynamics properties of the Edinburgh VSA.
 * \ingroup edinburghvsa
 * \sa <a href="../m2html/MACCEPA/m-files/dynamics/index.html">Matlab wrappers to this library</a>
 */
#include <libedinburghvsa.h>

/**
 * \brief Three dimensional cross product
 * \param[out] c cross product
 * \param[in] a,b vectors to cross
 */
	void
cross ( double a[3], double b[3], double c[3] )
{
	c[0] = a[1]*b[2] - a[2]*b[1];
	c[1] = a[2]*b[0] - a[0]*b[2];
	c[2] = a[0]*b[1] - a[1]*b[0];

	return ;
}		/* -----  end of function cross  ----- */


/** \brief Initialise model struct (set default parameters).
 *  \param model model struct.
 */
void edinburghvsa_model_init(edinburghvsa_model *model) {
	model->inertia          = INERTIA;
   	model->spring_constant  = SPRING_CONSTANT;
	model->spring_rest_length = SPRING_REST_LENGTH;
	model->lever_length     = LEVER_LENGTH;
	model->link_lever_length = LINK_LEVER_LENGTH;
	model->joint_to_motor_axis_x_separation = JOINT_TO_MOTOR_AXIS_X_SEPARATION;
	model->joint_to_motor_axis_y_separation = JOINT_TO_MOTOR_AXIS_Y_SEPARATION;
    model->damping_constant = DAMPING_CONSTANT;
    model->gravity_constant = GRAVITY_CONSTANT;
    model->viscous_friction = VISCOUS_FRICTION;
    model->coulomb_friction = COULOMB_FRICTION;
    model->umax[0]          = U_ULIM_RAD_SERVO0;
    model->umax[1]          = U_ULIM_RAD_SERVO1;
    model->umin[0]          = U_LLIM_RAD_SERVO0;
    model->umin[1]          = U_LLIM_RAD_SERVO1;
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
void edinburghvsa_model_get_acceleration ( double * acc, double * x, double * u, edinburghvsa_model * model ) {

	double I = model->inertia;
	double tau;
 	edinburghvsa_model_get_torque( &tau, x, u, model );
 	acc[0] = tau/I;

	return;
}

/** 
 *  \brief Calculate total joint torque as a function of current state and command. 
 *  \param[out] tau torque
 *  \param[in]  x state (postion, velocity)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  This implements \f$ \tau(\mathbf{x},\mathbf{u}) = \tau_{actuators}(q,\mathbf{u}) - \tau_{damping} - \tau_{gravity} - \tau_{friction} \f$
 *
 */
void edinburghvsa_model_get_torque( double * tau, double * x, double * u, edinburghvsa_model * model ) {

	double tau_actuator;  edinburghvsa_model_get_actuator_torque ( &tau_actuator, x, u, model );
	double tau_damping ;  edinburghvsa_model_get_damping_torque  ( &tau_damping , x, u, model );
	double tau_gravity ;  edinburghvsa_model_get_gravity_torque  ( &tau_gravity , x, u, model );
	double tau_friction;  edinburghvsa_model_get_friction_torque ( &tau_friction, x, u, model );

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
 *  \f$ \tau(q,\mathbf{u}) = \mathbf{\hat{z}}^\top(\mathbf{a}_1\times\mathbf{F}_1+\mathbf{a}_2\times\mathbf{F}_2)\f$
 *
 *  where 
 *  \li \f$\mathbf{F}_1,\mathbf{F}_2\f$ are spring forces with \f$\mathbf{F}_i(q,u_i)=\kappa(s_i-s_0)\hat{\mathbf{s}}_i\quad i\in\{1,2\}\f$,
 *  \li \f$\mathbf{s}_1,\mathbf{s}_2\f$ are the vectors along the springs \f$\mathbf{s}_i=\mathbf{s}_i(q,u_i)\quad i\in\{1,2\}\f$,
 *  \li \f$\mathbf{a}_1,\mathbf{a}_2\f$ are moment arms \f$\mathbf{a}_i=\mathbf{a}_i(q)\quad i\in\{1,2\}\f$.
 *
 *
 * \note This calculation is based on the derivation given in:
 *   Djordje Mitrovic, Stefan Klanke and Sethu Vijayakumar,
 *   Learning Impedance Control of Antagonistic Systems based
 *   on Stochastic Optimisation Principles, International
 *   Journal of Robotic Research, Vol. 30, No. 5, pp. 556-573
 *   (2011).
 *
 */
void edinburghvsa_model_get_actuator_torque( double * tau, double * x, double * u, edinburghvsa_model * model ) {

	/* get model parameters */
	double a = model->link_lever_length;
	double L = model->lever_length;
	double h = model->joint_to_motor_axis_x_separation;
	double d = model->joint_to_motor_axis_y_separation;
	double K = model->spring_constant;
	double r = model->spring_rest_length;

	double cosq = cos(x[0]);
	double sinq = sin(x[0]);
	double  a1[3]={-a*cosq,-a*sinq,0};
	double  a2[3]={ a*cosq, a*sinq,0};
	double  s1[3]={-h-L*sin(u[0])-a1[0],-d+L*cos(u[0])-a1[1],0};
	double  s2[3]={ h+L*sin(u[1])-a2[0],-d+L*cos(u[1])-a2[1],0};
	double norms1=sqrt(pow(s1[0],2)+pow(s1[1],2));
	double norms2=sqrt(pow(s2[0],2)+pow(s2[1],2));
	double  F1[3]={K*(norms1-r)*(s1[0]/norms1),K*(norms1-r)*(s1[1]/norms1),0};
	double  F2[3]={K*(norms2-r)*(s2[0]/norms2),K*(norms2-r)*(s2[1]/norms2),0};

	tau[0] = a1[0]*F1[1]-a1[1]*F1[0] + a2[0]*F2[1]-a2[1]*F2[0];

/* 	double c1[3],c2[3];
 * 	cross(a2,F1,c1);
 * 	cross(a2,F2,c2);
 * 	tau[0]=c2[2]-c1[2];
 */

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
 */
void edinburghvsa_model_get_damping_torque( double * tau, double * x, double * u, edinburghvsa_model * model ) {

	double bc= model->damping_constant;
 	tau[0] = bc*x[1];

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
void edinburghvsa_model_get_gravity_torque( double * tau, double * x, double * u, edinburghvsa_model * model ) {

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
void edinburghvsa_model_get_friction_torque( double * tau, double * x, double * u, edinburghvsa_model * model ) {

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
 *  \f$k(q,\mathbf{u})=-\frac{\partial\tau}{\partial q}|_{q}\f$ 
 *
 *  where \f$\tau\f$ is the actuator torque (i.e., torque due to 
 *  the variable stiffness mechanism).
 *
 */
void edinburghvsa_model_get_stiffness ( double * k, double * x, double * u, edinburghvsa_model * model ) {

	/* get model parameters */
	double a = model->link_lever_length;
	double L = model->lever_length;
	double h = model->joint_to_motor_axis_x_separation;
	double d = model->joint_to_motor_axis_y_separation;
	double K = model->spring_constant;
	double r = model->spring_rest_length;

	double cosq = cos(x[0]);
	double sinq = sin(x[0]);
	double  a1[3]={-a*cosq,-a*sinq,0};
	double  a2[3]={ a*cosq, a*sinq,0};
	double  s1[3]={-h-L*sin(u[0])-a1[0],-d+L*cos(u[0])-a1[1],0};
	double  s2[3]={ h+L*sin(u[1])-a2[0],-d+L*cos(u[1])-a2[1],0};
	double norms1=sqrt(pow(s1[0],2)+pow(s1[1],2));
	double norms2=sqrt(pow(s2[0],2)+pow(s2[1],2));
	double  F1[3]={K*(norms1-r)*(s1[0]/norms1),K*(norms1-r)*(s1[1]/norms1),0};
	double  F2[3]={K*(norms2-r)*(s2[0]/norms2),K*(norms2-r)*(s2[1]/norms2),0};

	int i;
	double alpha[3];
	double phi[3];
	for ( i = 0; i < 3; i += 1 ) {
		alpha[i]=a2[i];
		phi  [i]=F2[i]-F1[i];
	}

  	double dalphadq[3]={-a*sinq,a*cosq,0};

	double dF1dq[3];
	dF1dq[0]=K*( r/pow(norms1,3)*(s1[0]*(-a*sinq)+s1[1]*(a*cosq))*s1[0] + (1-r/norms1)*(-a*sinq) );
	dF1dq[1]=K*( r/pow(norms1,3)*(s1[0]*(-a*sinq)+s1[1]*(a*cosq))*s1[1] + (1-r/norms1)*( a*cosq) );
	dF1dq[2]=K*( r/pow(norms1,3)*(s1[0]*(-a*sinq)+s1[1]*(a*cosq))*s1[2] + (1-r/norms1)*       0  );

	double dF2dq[3];
	dF2dq[0]=K*(r*pow(norms2,-3)*(s2[0]*(a*sinq)+s2[1]*(-a*cosq))*s2[0]+(1-r*pow(norms2,-1))*( a*sinq));
	dF2dq[1]=K*(r*pow(norms2,-3)*(s2[0]*(a*sinq)+s2[1]*(-a*cosq))*s2[1]+(1-r*pow(norms2,-1))*(-a*cosq));
	dF2dq[2]=K*(r*pow(norms2,-3)*(s2[0]*(a*sinq)+s2[1]*(-a*cosq))*s2[2]+(1-r*pow(norms2,-1))*       0 );

	double dphidq[3];
	for ( i = 0; i < 3; i += 1 ) {
		dphidq[i]=dF2dq[i]-dF1dq[i];
	}

	double c1[3],c2[3];
	cross(dalphadq,phi,c1);
	cross(alpha,dphidq,c2);
	k[0]=-(c1[2]+c2[2]);

/* 	
 *  finite difference calcualtion:
 *
 *  double delta=1e-6;
 * 	double taup,taum,xd[DIMX];
 * 
 * 	memcpy(xd,x,2*sizeof(double)); xd[0]+=delta; edinburghvsa_model_get_actuator_torque(&taup,xd,u,model);
 * 	memcpy(xd,x,2*sizeof(double)); xd[0]-=delta; edinburghvsa_model_get_actuator_torque(&taum,xd,u,model);
 * 	k[0] = -(taup-taum)/(2*delta);
 */

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
void edinburghvsa_model_get_stiffness_jacobian ( double * J, double * x, double * u, edinburghvsa_model * model ) {
	double delta=1e-6;
	double kp,km,ud[DIMU];

	memcpy(ud,u,2*sizeof(double)); ud[0]+=delta; edinburghvsa_model_get_stiffness(&kp,x,ud,model);
	memcpy(ud,u,2*sizeof(double)); ud[0]-=delta; edinburghvsa_model_get_stiffness(&km,x,ud,model);
	J[0] = (kp-km)/(2*delta);

	memcpy(ud,u,2*sizeof(double)); ud[1]+=delta; edinburghvsa_model_get_stiffness(&kp,x,ud,model);
	memcpy(ud,u,2*sizeof(double)); ud[1]-=delta; edinburghvsa_model_get_stiffness(&km,x,ud,model);
	J[1] = (kp-km)/(2*delta);

	return;
}     

/** \brief Calculate equilibrium position as a function of current state and command.
 *  \param[out] q0 joint equilibrium position
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *
 *  \todo This uses a line search around the current state. Efficiency could be improved by deriving the roots where tau=0 analytically.
 *
 */
void edinburghvsa_model_get_equilibrium_position ( double * q0, double * x, double * u, edinburghvsa_model * model ) {

	double last, step, tau;
	double xt[DIMX] = {x[0],x[1]};
	int iter = 0;

	last = 0.0;
	step = 0.1;

	while(++iter<100) {
		edinburghvsa_model_get_actuator_torque( &tau, xt, u, model );
		if (fabs(tau) < 1e-8){
			q0[0]=xt[0];
			return;
		}
		if (tau>0) {
			xt[0] += step;
			if (last < 0) step*=0.5;
		} else {
			xt[0] -= step;
			if (last > 0) step*=0.5;
		}
		last = tau;
	}
#ifdef MEX_INTERFACE
	mexWarnMsgTxt(
#else
	fprintf(stderr,
#endif
	"Warning, eq. position calculation may be inaccurate!");

	q0[0]=xt[0];

	return;
}     

/** \brief Calculate Jacobian of equilibrium position with respect to motor commands (as a function of current state and command).
 *  \param[out] J joint equilibrium position
 *  \param[in]  x state (velocity, acceleration)
 *  \param[in]  u command (motor positions in radiens)
 *  \param[in]  model model struct
 *  \note This calculation is based on finite differences.
 */
void edinburghvsa_model_get_equilibrium_position_jacobian ( double * J, double * x, double * u, edinburghvsa_model * model ) {
	double delta=1e-6;
	double q0p,q0m,ud[DIMU];

	memcpy(ud,u,2*sizeof(double)); ud[0]+=delta; edinburghvsa_model_get_equilibrium_position(&q0p,x,ud,model);
	memcpy(ud,u,2*sizeof(double)); ud[0]-=delta; edinburghvsa_model_get_equilibrium_position(&q0m,x,ud,model);
	J[0] = (q0p-q0m)/(2*delta);

	memcpy(ud,u,2*sizeof(double)); ud[1]+=delta; edinburghvsa_model_get_equilibrium_position(&q0p,x,ud,model);
	memcpy(ud,u,2*sizeof(double)); ud[1]-=delta; edinburghvsa_model_get_equilibrium_position(&q0m,x,ud,model);
	J[1] = (q0p-q0m)/(2*delta);

	return;
}

/** \brief Calculate motor positions based on finite impulse response filters. 
 *  \param[out] m motor positions
 *  \param[in]  x filter states (velocity, acceleration)
 *  \param[in]  model model struct
 * \todo This function is no longer used and should be removed.
 */
void edinburghvsa_model_get_motor_positions ( double * m, double * x, edinburghvsa_model * model ) {

	int i;

	m[0]=0;
	m[1]=0;
	for ( i = 0; i < FILTER_DIMENSION; i += 1 ) {
		m[0]+=model->b_filter[i]*x[i];
		m[1]+=model->b_filter[i+FILTER_DIMENSION]*x[i+FILTER_DIMENSION];
	}

	return;
}

