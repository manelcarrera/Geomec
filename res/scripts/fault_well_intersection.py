import logging
import os
import sys
import numpy as np

from fractions import *

import time

LOG_FILE=os.path.dirname(os.path.abspath(__file__)) + "py.log"

# Algorithm to check if two boxes overlap
# https://stackoverflow.com/questions/20925818/algorithm-to-check-if-two-boxes-overlap

import ntpath
def handle_exception():
	exc_type, exc_value, exc_traceback = sys.exc_info()
	print('exc_type:{}'.format(exc_type))
	print('exc_value:{}'.format(exc_value))
	print('exc_traceback.tb_lineno:{}'.format(exc_traceback.tb_lineno))
	traceback_details = {
		'filename': ntpath.basename( exc_traceback.tb_frame.f_code.co_filename ),
		'lineno'  : exc_traceback.tb_lineno,
		'function': exc_traceback.tb_frame.f_code.co_name,
		'type'    : exc_type.__name__,
		'message' : exc_value.message, # or see traceback._some_str() 
	}
	logging.error('Exception:{}'.format( traceback_details ) )


def signed_volume(a,b,c,d):
	#print('6a')

	cross_=np.cross( b-a, c-a )
	#print('cross:{}'.format(cross_))

	dot_=np.dot( cross_, d-a )
	#print('dot:{}'.format(dot_))

	#print('1/6={}'.format(1.0/6.0))

	r = ( 1.0/6.0 )*np.dot( 
		np.cross( b-a, c-a ), 
		d-a )
	#print('6b')
	return r

''' intersect face and line'''
def intersects( p1, p2, p3, q1, q2 ):

	#print('p1:{} p2:{} p3:{} -- q1:{} q2:{}',p1,p2,p3,q1,q2)

	#: for intersection: 
	# (1) different sign 
	# (2) same sign

	# 1
	a = signed_volume(q1,p1,p2,p3)
	b = signed_volume(q2,p1,p2,p3)

	# 1
	c = signed_volume(q1,q2,p1,p2)
	d = signed_volume(q1,q2,p2,p3)
	e = signed_volume(q1,q2,p3,p1)

	#print('res: a:{} b:{} c:{} d:{} e:{}'.format(a,b,c,d,e))


	if ( (a>=0 and b<0) or (a<0 and b>=0) ) and  ( ( c<0 and d<0 and e<0) or (c>=0 and d>=0 and e>=0) ):
		return True # intersects
	else:
		return False



def parse( file ):

	points_map={}
	elems_map={}
	elem=[None]*3
	well_l=[]

	with open( file ) as inf:
		points = False
		elems = False
		well = False
		for line_ in inf:
			line = line_.rstrip()
			#print('line:\'{}\''.format(line))
			
			#must be at the beginning
			if not line:
				#print('EMPTY Line')
				points=False
				elems=False
				well=False
				continue

			#if line == 'POINTS':
			if line.startswith('Index'):
				points=True
				continue

			#if line == 'ELEMENTS':
			if line.startswith('ElementNr'):
				elems=True
				i=0
				continue

			if line.startswith('Easting'):
				well=True
				continue

			if points == True:
				fields = line.split('\t')
				#print('fileds:{}'.format(fields))
				points_map[ fields[0] ] = [ float(fields[1]), float(fields[2]), float(fields[3]) ]

			if elems == True:
				fields = line.split('\t')
				#print('fields:{}'.format(fields))
				if i<3:
					elem[i] = fields[ 1 ]
				if i == 3:
					elems_map[ fields[0] ] = elem
				if i == 5:
					i=-1 # Atention !!!
					elem=[None]*3

				i+=1

			if well == True:
				fields = line.split('\t')
				#print('fileds:{}'.format(fields))
				well_l.append( [ float(fields[0]), float(fields[1]), float(fields[2]) ] )

	#print('points:{}'.format( points_map ))
	#print('')
	#print('elems:{}'.format( elems_map ))
	#print('')
	#print('well:{}'.format( well_l ))

	return points_map, elems_map, well_l
	#return elems_map


'''from nodes id to points'''
def coordinates( v, points_map ):

	p=points_map[ v[0] ]
	p1 = np.array([ p[0], p[1], p[2] ])

	p=points_map[ v[1] ]
	p2 = np.array([ p[0], p[1], p[2] ])

	p=points_map[ v[2] ]
	p3 = np.array([ p[0], p[1], p[2] ])

	return p1,p2,p3


def find_( elems_map, points_map, line ):

	try:

		q1=line[0]
		q2=line[1]

		elems=[]


		for k, v in elems_map.iteritems():

			p1,p2,p3=coordinates( v, points_map )
			#print('p1:{} p2:{} p3:{}',p1,p2,p3)

			res=intersects( p1, p2, p3, q1, q2 )
			if res:
				elems.append( k )

		#print('find_ : elems:{}'.format(elems))
		return elems

	except:
		print('except')
		handle_exception()

def test_01():

	#name = input("What's your name? ")

	#############################
	# 1. IN

	# () -> tuple

	# triangle
	p1 = np.array([0,0,0])
	p2 = np.array([0,3,0])
	p3 = np.array([3,0,0])

	# line
	q1 = np.array([1,1,1])
	q2 = np.array([1,1,-1])

	res=intersects( p1, p2, p3, q1, q2 )

	if res:
		print('Intersects')
	else:
		print('DONT')


	#############################
	# 2. OUT

	# line
	q1 = np.array([5,5,1])
	q2 = np.array([5,5,-1])

	res=intersects( p1, p2, p3, q1, q2 )

	if res:
		print('Intersects')
	else:
		print('DONT')


def bbox( points ):

    # bl: botton-left
    # tr: top-right

    bl_x = min( point[0] for point in points )
    bl_y = min( point[1] for point in points )
    bl_z = min( point[2] for point in points )

    tr_x = max( point[0] for point in points )
    tr_y = max( point[1] for point in points )
    tr_z = max( point[2] for point in points )

    return [ (bl_x, bl_y, bl_z), (tr_x, tr_y, tr_z) ]


# 0: min, 1: max
# 0: x, 1: y, 2: z

#box1 = (xmin1, xmax1)
#box2 = (xmin2, xmax2)
def overlapping_1D( b1, b2 ): 

	#return (xmax1 >= xmin2 and xmax2 >= xmin1)
	#return (b1[1][0] >= b2[0][0]) and (b2[1][0] >= b1[0][0])
	return (b1[1] >= b2[0]) and (b2[1] >= b1[0])


# TODO: verify this before uncomment

#box1 = (x:(xmin1,xmax1),y:(ymin1,ymax1))
#box2 = (x:(xmin2,xmax2),y:(ymin2,ymax2))
#def overlapping_2D( b1, b2 ): 
	#overlapping2D(box1,box2) = overlapping1D(box1.x, box2.x) and 
	#                           overlapping1D(box1.y, box2.y)

#	return overlapping_1D( (b1[0][0], b1[1][0]), (b2[0][0], b2[1][0]) )  and overlapping1D( (b1[0][1], b1[1][1]), (b2[0][1], b2[1][1]) )


def overlapping_3D( b1, b2 ): 

	#box1 = (x:(xmin1,xmax1),y:(ymin1,ymax1),z:(zmin1,zmax1))
	#box2 = (x:(xmin2,xmax2),y:(ymin2,ymax2),z:(zmin2,zmax2))
	#overlapping3D(box1,box2) = overlapping1D(box1.x, box2.x) and 
	#                           overlapping1D(box1.y, box2.y) and
	#                           overlapping1D(box1.z, box2.z)

	return overlapping_1D( (b1[0][0], b1[1][0]), (b2[0][0], b2[1][0]) ) and overlapping_1D( (b1[0][1], b1[1][1]), (b2[0][1], b2[1][1]) ) and overlapping_1D( (b1[0][2], b1[1][2]), (b2[0][2], b2[1][2]) )


def main():

	try:

		logging.basicConfig( 
			filename=LOG_FILE, 
			format='%(asctime)s %(message)s', 
			level=logging.DEBUG, filemode='w' )

		start = time.time()

		#infile = sys.argv[1]
		#file = 'test1_tetra_fault.dat'

		# DONT overlps		
		#file = 'SWR_fault_and_well.dat'

		# overlaps
		#file = 'input.dat'
		
		file = sys.argv[1] if len(sys.argv) > 1 else 'input.dat'
		print('File: {}'.format(file))
		
		points_map, elems_map, well_l = parse( file )

		# fault
		min_,max_=bbox( points_map.values() )
		print('Fault: min:{} max:{}'.format( min_,max_ )) 

		# well
		min2_,max2_=bbox( well_l )
		print('Well: min:{} max:{}'.format( min2_,max2_ )) 

		# fault and well overlaps? 
		res=overlapping_3D( (min_,max_), (min2_,max2_) )

		if res:
			print('Overlaps') 
		else:
			print('NO overlaps') 
			return


		# line
		# TODO: 
		# 	needs to be taken from the file
		# 	lines with several segments not implemented yet

		elems=[]
		next_=None
		l = len( well_l )
		for index, point in enumerate( well_l ):
			if index < (l - 1):
				next_ = well_l[ index + 1 ]
				segment=[ np.array(point), np.array(next_) ]

				# -----------------------------------------------------
				# exit if DONT overlap
				# well
				min2_,max2_=bbox( [point,next_] )
				#print('segment: min:{} max:{}'.format( min2_,max2_ )) 

				# fault and well segment overlaps? 
				res=overlapping_3D( (min_,max_), (min2_,max2_) )

				if res:
					pass
					print('Well segment overlaps') 
				else:
					#print('Well segment DONT overlaps') 
					continue
				# -----------------------------------------------------

				elems1 = find_( elems_map, points_map, segment )
				if len( elems1 ):
					elems.append( elems1 )

		if len(elems):
			#for elem in elems:
			#	print('Intersects ... elem: \'{}\''.format( elem ) ) 
			print('Intersects ... elems:{}'.format( elems ) ) 
		else:
			print('NO Intersections' ) 

		end = time.time()
		print('Time: {}s'.format( end - start) )


		#q1 = np.array([50,50,0])
		#q2 = np.array([50,50,200])
		#line=[q1,q2]

		#elems = find_( elems_map, points_map, line )

		#for elem in elems:
		#	print('Intersects ... elem: \'{}\''.format( elem ) ) 



		#test_01()


	except:
		print('except')
		handle_exception()

if __name__=='__main__':

	main()