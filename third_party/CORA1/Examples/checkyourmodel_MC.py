# By Famke Kraaijeveld, last edited 15 June 2014
#
import sys
import pylab as p
import time

#--------------------------------------------
# Your model
#--------------------------------------------
# import your model
model = __import__('MohrCoulomb')
No = 10000
paramvalues = [10100.,9340.,8070.,300.,25.,0.5,2100.,0.15,2310.]
resistances = [0.15, 1.]
#
#--------------------------------------------
# Validation of model
#--------------------------------------------
paramnames = model.get_input_parameters()
print "Parameters ", paramnames
respnames = model.get_response_parameters()
print "Responses ", respnames
#
# validate your model
if ( len(paramvalues) == len(paramnames) and len(resistances) == len(respnames) ):
    #
    # Ones
    respvalues = model.get_response(*paramvalues)
    print 'Response values ', respvalues
    for rindex in range(len(respnames)):
	   LSF = model.get_zvalue(respnames[rindex], respvalues, resistances[rindex])
	   print 'LSF values for ', respnames[rindex], LSF
    #--------------------------------------------
    # TIMER
    #--------------------------------------------
    start = time.time()
    print start, 'Starting sampling ', No
    for nindex in range(No):
	resp = model.get_response(*paramvalues)
	for rindex in range(len(respnames)):
	   LSF = model.get_zvalue(respnames[rindex], resp, resistances[rindex])

    end = time.time()	
    print end, 'Finished sampling ', No
    print 'Elapsed time ', end - start
