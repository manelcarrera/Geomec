# mohrinflation.py
"""Mohr circle analysis to be used for Reliability Analysis

File includes modules:
		get_response_parameters		: returns the names of calculation results
		get_input_parameters 		: returns the names of parameters for the evaluation
		get_response			: calculate an array of calculation results
		get_zvalue			: returns the with response associate failure 

"""
#
# Declarations
import numpy as np
#
# ------------------------
# get_response
# ------------------------
def get_response(S1_ini,S3_ini,Pp_ini,S0,phi,gam3_depl,DEPL,gam3_infl,INFL):
	"""
	Calculates the response values based on Mohr-Coulomb analysis for responses
	1. SR - stress ratio at given injection pressure
	2. SCU - onset of shear failure given injection pressure INFL_in

	The parameters used are defined by
    
	S1_ini - maximum pre-production stress
	S3_ini - minimum pre-production stress
	Pp_ini - initial pore pressure
	S0 - cohesion
	phi - friction angle
	
	DEPL - depletion pressure (positive for depletion)
	gam3_depl - change in S3 per unit change in pore pressure during depletion
	INFL - injection pressure (positive for injection)
	gam3_infl - change in S3 per unit change in pore pressure during injection
	
	
	"""
	### --------------------------
	### VALIDATION
	### --------------------------
	errval = float('NaN')
	# Initial stresses and pressure
	if (Pp_ini < 0. or S1_ini < Pp_ini or S3_ini < Pp_ini):
		# raise ValueError('Initial stress and pressure combination not physical')
		validationOK = 0
	# Final stresses
	elif (0.0 > gam3_depl or gam3_depl > 1.0 or  0.0 > gam3_infl or gam3_infl > 1.0):
		# raise ValueError('Stress path coefficients nonphysical')
		validationOK = 0
	elif (DEPL < 0. or INFL < 0.):
		# raise ValueError('Depletion pressure DEPL is assumed positive for depletion and Injection pressure change is assumed positive for injection.')
		validationOK = 0
	# Failure envelop
	elif (S0 < 0. or phi <= 0. or phi >= 90.):
		# raise ValueEror('Input for failure envelop not physical not physical')
		validationOK = 0
	else:
		validationOK = 1
	#
	#
	### --------------------------
	### RESPONSE CALCULATION
	### --------------------------
	if not validationOK == 1:
		resp = [errval, errval]
	else:
		## because of sampling S3 can be larger than S1
		## make sure you use largest and smallest principal stress correctly
		if (S3_ini > S1_ini):
		  temp = S3_ini
		  S3_ini = S1_ini
		  S1_ini = temp
		#
		## Basic constants Mohr circle analysis
		phirad = phi*np.pi/180.
		#
		## Stress ratio
		Sig1_eff = S1_ini - (Pp_ini-DEPL+INFL)
		Sig3_eff = (S3_ini-(DEPL*gam3_depl)+INFL*gam3_infl) - (Pp_ini-DEPL+INFL)
		SR = Sig3_eff / Sig1_eff
		#
		## SCU
		tau = ( Sig1_eff - Sig3_eff )/2.
		sig_a = ( Sig1_eff + Sig3_eff )/2.
		taumax = S0 * np.cos( phirad ) + sig_a * np.sin( phirad )
		if taumax <= 0:
			SCU = 10.
		else:
			SCU = tau/taumax  
		#
		#
		### RESPONSE CALCULATION
		resp = [SR, SCU]
		#
	return resp
#
# ------------------------
# get_input_parameters
# ------------------------
def get_input_parameters():
	"""
	Returns an array of names of input parameters
	"""
	return ["S1_ini","S3_ini","Pp_ini","S0","phi","gam3_depl","DEPL","gam3_infl","INFL"]
#
# ------------------------
# get_response_parameters
# ------------------------
def get_response_parameters():
	"""
	Returns an array of names of calculation responses
	"""
	return ["SR", "SCU"]

#
# ------------------------
# get_zvalue
# ------------------------
def get_zvalue(respname, resp, resistance):
	"""
	Returns one value for failure (LSV)
	Input:
	 - name of the response
	 - response value
	 - failure limit or resistance
	 
	Output:
	LSF = SR - resistance: fails when operating Stress Ratio < critical stress ratio (resistance)
	LSF = resistance - SCU: fails when operating SCU >= allowable SCU (resistance)
	"""
	### --------------------------
	### FAILURE CALCULATION
	### --------------------------
	if respname == "SR" : 
		LSF = resp[0] - resistance
	elif respname == "SCU" : 
		LSF = resistance - resp[1]
	else:
		raise ValueError("response name unknown") 

	#print "LSF = ", LSF
	return LSF
