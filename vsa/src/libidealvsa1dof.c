/** \defgroup idealVsa1dof Ideal 1-DOF VSA
 */
/** \page ideal_vsa_1dof_information
 *
 * \section ideal_vsa_1dof_info Information about the ideal
 * 1-DOF VSA
 *
 * \subsection dynamics Dynamics
 *
 * The ideal 1-DOF VSA is an idealised model of a 1-DOF VSA
 * robot. The geometry of this model is shown below: \image
 * html ideal_vsa_1dof_model.png
 *
 * The dynamics equations are as follows.
 *
 * \li Joint torque: \f$\tau(\mathbf{x},\mathbf{u}) = u_1
 * (q-u_0)\f$
 *
 * \li Joint stiffness:  \f$k=u_1\f$
 *
 * \li Equilibrium Position: \f$q_{eq}=u_0\f$
 *
 * \sa The maxima script doc/maxima/ideal_vsa_1dof.mac for
 * derivations.
 *
 * \subsection system_characterisation System
 * Characterisation
 *
 * Based on this model of the dynamics, the below figures
 * give some characterisation of the system dynamics.
 * 
 * \image html eqpos_vs_u_ideal_vsa_1dof.png "Equlibrium position vs. motor positions (evaluated at q=0, dq/dt=0)"
 * \image html stiff_vs_u_ideal_vsa_1dof.png "Stiffness vs. motor positions (evaluated at q=0, dq/dt=0)"
 *
 * \ingroup idealVsa1dof
*/

/** \file libidealvsa1dof.c 
 * \brief Dummy file containing documentation only.
 *  \ingroup idealVsa1dof
 */

