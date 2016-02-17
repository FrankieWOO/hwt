/** 
 * \file mex_libmaccepa.c
 * \author Matthew Howard (MH), matthew.howard@ed.ac.uk
 * \date August 2010
 * \brief MEX gateway to MACCEPA dynamics library (libmaccepa).
 * \ingroup MACCEPA
 */
#include <mex.h>
#include <matrix.h>
#include <libmaccepa.h>

/** \brief struct containing model parameters. */
maccepa_model model;

/** \brief Mex interface usage message. */
static char usage_msg[]=
"Usage of MACCEPA model MEX interface:\n" \
"  model_maccepa('function',args) calls function 'function' with arguments args.\n See documentation for which functions are defined.\n" ;

/**
 * \brief Converts Matlab model struct into C model struct
 * \param[out] model C model struct
 * \param[in] matlab_model  Matlab model struct
 */
	void
mex_libmaccepa_model_matlab_to_c ( maccepa_model * model, const mxArray * matlab_model )
{
	memcpy(&(model->inertia                         ), mxGetPr(mxGetField(matlab_model,0,"I"                               )),      sizeof(double));
	memcpy(&(model->link_length                     ), mxGetPr(mxGetField(matlab_model,0,"L"                               )),      sizeof(double));
	memcpy(&(model->spring_constant                 ), mxGetPr(mxGetField(matlab_model,0,"spring_constant"                 )),      sizeof(double));
	memcpy(&(model->lever_length                    ), mxGetPr(mxGetField(matlab_model,0,"lever_length"                    )),      sizeof(double));
	memcpy(&(model->pin_displacement                ), mxGetPr(mxGetField(matlab_model,0,"pin_displacement"                )),      sizeof(double));
	memcpy(&(model->drum_radius                     ), mxGetPr(mxGetField(matlab_model,0,"drum_radius"                     )),      sizeof(double));
	memcpy(&(model->damping_constant                ), mxGetPr(mxGetField(matlab_model,0,"damping_constant"                )),      sizeof(double));
	memcpy(&(model->gravity_constant                ), mxGetPr(mxGetField(matlab_model,0,"gravity_constant"                )),      sizeof(double));
	memcpy(&(model->viscous_friction                ), mxGetPr(mxGetField(matlab_model,0,"viscous_friction"                )),      sizeof(double));
	memcpy(&(model->coulomb_friction                ), mxGetPr(mxGetField(matlab_model,0,"coulomb_friction"                )),      sizeof(double));
	memcpy(&(model->umax                            ), mxGetPr(mxGetField(matlab_model,0,"umax"                            )), DIMU*sizeof(double));
	memcpy(&(model->umin                            ), mxGetPr(mxGetField(matlab_model,0,"umin"                            )), DIMU*sizeof(double));

	return ;
}		/* -----  end of function mex_libmaccepa_model_matlab_to_c  ----- */

/**
 * \brief Converts C model struct into Matlab model struct
 * \param[out] matlab_model  Matlab model struct
 * \param[in] model C model struct
 */
	void
mex_libmaccepa_model_c_to_matlab ( mxArray * matlab_model, const maccepa_model * model )
{
	double dimQ = DIMQ; memcpy( mxGetPr(mxGetField(matlab_model,0,"dimQ")), &dimQ, sizeof(double));
	double dimU = DIMU; memcpy( mxGetPr(mxGetField(matlab_model,0,"dimU")), &dimU, sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"I"                               )), &(model->inertia                         ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"L"                               )), &(model->link_length                     ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"spring_constant"                 )), &(model->spring_constant                 ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"lever_length"                    )), &(model->lever_length                    ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"pin_displacement"                )), &(model->pin_displacement                ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"drum_radius"                     )), &(model->drum_radius                     ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"damping_constant"                )), &(model->damping_constant                ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"gravity_constant"                )), &(model->gravity_constant                ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"viscous_friction"                )), &(model->viscous_friction                ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"coulomb_friction"                )), &(model->coulomb_friction                ),      sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"umax"                            )), &(model->umax                            ), DIMU*sizeof(double));
	memcpy( mxGetPr(mxGetField(matlab_model,0,"umin"                            )), &(model->umin                            ), DIMU*sizeof(double));

	return ;
}		/* -----  end of function mex_libmaccepa_model_c_to_matlab  ----- */

/**
 * \brief Check that the arguments passed are correct. (In general the convention is y = f(x,u,model). )
 * \param[in] nrhs number of arguments
 * \param[in] prhs argument pointers
 * \returns true if arguments are ok, false if not. 
 */
	bool
mex_libmaccepa_check_arguments ( int nrhs, const mxArray *prhs[] )
{
	if (nrhs < 4) { 
		mexErrMsgTxt("Too few arguments.");
		return false;
	} 
	int dimX = mxGetM(prhs[1]);
	if (dimX != DIMX){
		mexErrMsgTxt("Wrong dimensionality of x.");
		return false;
	} 
	int dimU = mxGetM(prhs[2]);
	if (dimU != DIMU){
		mexErrMsgTxt("Wrong dimensionality of u.");
		return false;
	} 
	if (!mxIsStruct(prhs[3])){
		mexErrMsgTxt("You need to pass a struct containing a valid MACCEPA model.");
		return false;
	} 
	return true;
}		/* -----  end of function mex_libmaccepa_check_arguments  ----- */

/** 
 * \brief Mex gateway function. Provides access to C functions.
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	/* Not enough arguments. Print usage message and exit. */
	if (nrhs < 1) {
		printf(usage_msg);
		return;
	}

	maccepa_model_init ( &model);

	if (mxIsChar(prhs[0])) {
		char function[mxGetN(prhs[0])+1];
		mxGetString(prhs[0], function, mxGetN(prhs[0])+1);

		if(strcmp(function,"maccepa_model")==0){
			int i;
			/* define field names of matlab model struct */
			const char * fnames[16] = {
				"dimQ",
				"dimU",
				"I",
				"L",
				"spring_constant",
				"lever_length",
				"pin_displacement",
				"drum_radius",
				"damping_constant",
				"gravity_constant",
				"viscous_friction",
				"coulomb_friction",
				"umax",
				"umin",
				"m1_filter_parameters",
				"m2_filter_parameters"};
			plhs[0] = mxCreateStructMatrix(1, 1, 16, fnames);
			/* intialise fields */
			for ( i =  0; i < 12; i += 1 ) { mxSetFieldByNumber(plhs[0], 0, i, mxCreateDoubleMatrix(   1,1,mxREAL) ); }
			for ( i = 12; i < 14; i += 1 ) { mxSetFieldByNumber(plhs[0], 0, i, mxCreateDoubleMatrix(DIMU,1,mxREAL) ); }
			/* copy c model struct to matlab model struct */
			mex_libmaccepa_model_c_to_matlab ( plhs[0], &model );

			return;
		}
		else 
		{
			if ( !mex_libmaccepa_check_arguments(nrhs,prhs) ) return;
			mex_libmaccepa_model_matlab_to_c (&model,prhs[3]); /* get model parameters passed by user */

			if(strcmp(function,"maccepa_model_get_actuator_torque")==0){
				plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL); /* Create output vector */
				maccepa_model_get_actuator_torque (mxGetPr(plhs[0]), mxGetPr(prhs[1]), mxGetPr(prhs[2]), &model );
			}
			else if(strcmp(function,"maccepa_model_get_spring_force")==0){
				plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL); /* Create output vector */
				maccepa_model_get_spring_force (mxGetPr(plhs[0]), mxGetPr(prhs[1]), mxGetPr(prhs[2]), &model );
			}
			else if(strcmp(function,"maccepa_model_get_acceleration")==0){
				plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL); /* Create output vector */
				maccepa_model_get_acceleration (mxGetPr(plhs[0]), mxGetPr(prhs[1]), mxGetPr(prhs[2]), &model );
			}
			else if(strcmp(function,"maccepa_model_get_motor_positions")==0){
				plhs[0] = mxCreateDoubleMatrix(2,1,mxREAL); /* Create output vector */
				maccepa_model_get_motor_positions (mxGetPr(plhs[0]), mxGetPr(prhs[1]), &model );
			}
			else if(strcmp(function,"maccepa_model_get_equilibrium_position")==0){
				plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL); /* Create output vector */
				maccepa_model_get_equilibrium_position (mxGetPr(plhs[0]), mxGetPr(prhs[1]), mxGetPr(prhs[2]), &model );
			}
			else if(strcmp(function,"maccepa_model_get_equilibrium_position_jacobian")==0){
				plhs[0] = mxCreateDoubleMatrix(1,DIMU,mxREAL); /* Create output vector */
				maccepa_model_get_equilibrium_position_jacobian (mxGetPr(plhs[0]), mxGetPr(prhs[1]), mxGetPr(prhs[2]), &model );
			}
			else if(strcmp(function,"maccepa_model_get_stiffness")==0){
				plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL); /* Create output vector */
				maccepa_model_get_stiffness (mxGetPr(plhs[0]), mxGetPr(prhs[1]), mxGetPr(prhs[2]), &model );
			}
			else if(strcmp(function,"maccepa_model_get_stiffness_jacobian")==0){
				plhs[0] = mxCreateDoubleMatrix(1,DIMU,mxREAL); /* Create output vector */
				maccepa_model_get_stiffness_jacobian (mxGetPr(plhs[0]), mxGetPr(prhs[1]), mxGetPr(prhs[2]), &model );
			}
			else{
				printf(usage_msg);
			}
			return;
		}
	}

	printf(usage_msg);
}
