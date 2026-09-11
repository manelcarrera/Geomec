# mohrinflation.py
import sys
import numpy as np

def get_response(x1, x2):
	# BASE CASE VALUES
	resp = 1095.505812569 - 5*x1 - 5*x2
	#
	##
	### END
	return [resp]

def get_input_parameters():
	return ["x1","x2"]

def get_response_parameters():
	return ["LSF"]

def get_zvalue(respname, resp, resistance):
	#
	LSF = resp[0] - resistance
	return LSF

