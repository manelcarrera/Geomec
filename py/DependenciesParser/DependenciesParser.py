import os;
import sys;
LOG_FILE=os.path.dirname(os.path.abspath(__file__)) + "/depen_parser_py.log"

try:
	import logging
	import helper
	from shutil import copyfile

	try:
		f = open( LOG_FILE )
		timestamp = f.readline()[0:16].replace(':','-').replace(' ','_')
		dst = '{}_{}.log'.format( LOG_FILE[0:-4], timestamp )
		copyfile( LOG_FILE, dst )
	except:
		pass
	#logging.basicConfig( filename=LOG_FILE, format='%(asctime)s %(message)s', level=logging.DEBUG, filemode='w' )
	logging.basicConfig( filename=LOG_FILE, format='%(message)s', level=logging.DEBUG, filemode='w' )
	#logging.getLogger().setLevel( level=logging.DEBUG )
	logging.info('previous execution: {}'.format( dst ) )
	LOGGING_LEVEL_DICT={ logging.CRITICAL : 'CRITICAL', logging.ERROR : 'ERROR', logging.WARNING : 'WARNING', logging.INFO : 'INFO', logging.DEBUG : 'DEBUG' }
except:
	import logging
	logging.basicConfig( filename=LOG_FILE, format='%(asctime)s %(message)s', level=logging.DEBUG )
	import helper
	helper.handle_exception()

def gen_INCLUDEPATH():
	try:
		#in_ = sys.argv[1]
		#in_ = 'E:\dev\Geomec_ALLINCLUDES\qmake\basepth.pri'
		#in_='E:/dev/Geomec_ALLINCLUDES/vsprops/BuildCommon.props'
		in_='c:/aWork/aGeomec/Geomec_ALLINCLUDES/vsprops/BuildCommon.props'

		#out = sys.argv[2]

		try:
			file_i=open( in_, 'r' )
			#file_o = open( out, 'a' )

			tag_ini='APPS & LIBS'
			tag_end='LIBS GROUPS'

			tag_ini_found=False

			name=''

			d={}

			for line in file_i.readlines():

				if tag_ini_found:

					if tag_end in line:
						break								# everything parsed so skip
					else:
						l=line.strip()

						if l.startswith('<!--'):			# skip comment
							pass 
						else:								# not comment
							if len( l ) > 1:
								if l[ 0 ] == '<': 
									if l.startswith('</'):	# end var
										name=''
										logging.debug('')
										file_o.close()
									else:					# ini var
										name = l[1:-1]
										d[name]=[]
										logging.debug('{}:'.format(name))
										file_o = open( 'pri/'+name+'.pri', 'w' )
								else:
									text=l.replace('$(','$$').replace(')','')
									text2=text[0:text.find(';')]				# remove ';' and comments: <!-- comment -->
									text3='{} {}'.format('INCLUDEPATH +=', text2)
									logging.debug(text3)
									file_o.write(text3+'\n')
									d[name]+=text

				else:										# skip first lines till BASE vars block
					if tag_ini in line:
						tag_ini_found = True

			#logging.debug( '{}'.format( d ) )

				#address = mail.split(',')[1].strip()
				#out_file.write(address+',') #if you want to use commas to seperate the files, else use something like \n to write a new line.
		finally:
			file_i.close()
			#out_file.close()

		
	except:
		helper.handle_exception()
def main():

	try:
		logging.debug("------------------------------------------------------------------------------------------- IT : START")
		gen_INCLUDEPATH()
		logging.debug("/////////////////////////////////////////////////////////////////////////////////////////// IT : END ")
	except:
		helper.handle_exception()

if __name__=='__main__':
	main()
