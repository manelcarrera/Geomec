#!/usr/bin/env python

import sys
import ntpath
import logging

def show_error_details():
	'''shows exception extended info'''
	exc_type, exc_value, exc_traceback = sys.exc_info()
	traceback_details = {
		'filename': ntpath.basename( exc_traceback.tb_frame.f_code.co_filename ),
		'lineno'  : exc_traceback.tb_lineno,
		'function': exc_traceback.tb_frame.f_code.co_name,
		'type'    : exc_type.__name__,
		'message' : exc_value.message, # or see traceback._some_str() 
	}
	logging.error('Exception:{}'.format( traceback_details ) )

#
# include
#
_a=[
	#
	# a) not model related -> ALL OK, also in non-dsa
	#
	'MaterialParamTest.*',										# 0
	'SanityCheck.*',											# 1
	'SanityChecks.*',											# 2
	'ModelOperationsShell/ModelOperationsShell.*',				# 3
	'MainRun/*',												# 4
	'Gocads/Fw51GocadTest.*',									# 5
	'GocadWells/Fw51GocadWellTest.*',							# 6
	'Eclipses/Fw51EclipseTest.*',								# 7
	'*Run/Fw51TetMeshTest.*',									# 8
	#
	# b) model related
	#
	'ModelOperationsCORA.*',									# 9
	'ModelOperationsRGI.*',										# 10
	'ModelOperations.*', 										# 11
	'ModelOperationsBatch/ModelOperationsBatch.Scripts/*',		# 12
	#	
	'ModelOperationsIpc.*', 									# 13
	'ModelOperationsExport.*',									# 14
	'ModelOperationsDsa.*',										# 15
]
#
# exclude
#
_b=[
	'ModelOperationsRGI.RGI', 									# 0
	'ModelOperationsRGI.LogMessages',							# 1
	'ModelOperationsRGI.MonitoringPoints',						# 2
	#		
	'ModelOperations.ResultCache',								# 3
	'ModelOperations.HDF5Test',									# 4
	#
	'ModelOperations.ResultsBranched',							# 5
	'ModelOperations.HexaMeshRefinement',						# 6
	'ModelOperations.StartDiana', 								# 7
	'ModelOperations.dxw_and_mc41',								# 8
	#
	'ThirdRun/Fw51TetMeshTest.ReadFile/0',						# 9
	#
	'ModelOperationsBatch/ModelOperationsBatch.Scripts/*'		# 10
]


class GTestFilter:
	@staticmethod
	def join(a,include):
		a_=[ a[i] for i in include ]
		a__=':'.join(a_)
		return a__
	@staticmethod
	def get(include,exclude):
		a=GTestFilter.join(_a,include)
		b=GTestFilter.join(_b,exclude)

		if not b:
			c=a
		else:
			c='{}:-{}'.format(a,b)

		print c
	#
	# 
	#
	@staticmethod
	def get_(id):
		#
		# special case:
		# not model related excludign the long one
		#
		if id==20:	
			GTestFilter.get( [0,1,2,3,4,5,6,7,8], [9] )
		#
		# regular case: run one tests group
		#
		if id in range(0,16): # from CORA to the end
			GTestFilter.get( [id], [] )
		else:
			pass # TODO: to be implemented

def main(argv):
	try:
		id=int(argv[0])
		GTestFilter.get_(id)
	except Exception as exception:
		show_error_details()

if __name__=='__main__':
	try:
		main( sys.argv[1:] )
	except Exception as exception:
		show_error_details()