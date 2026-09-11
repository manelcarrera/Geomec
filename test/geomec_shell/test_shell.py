#!/usr/bin/env python

#python test_shell.py > res_test_shell_py.log 2>&1

# Comments, MCR-2019-11-17:
# To include this file into the installer we need to: 
# 1. include first 'Tests/Models/CORA' folder with all the models in the installer zip
# By now the only thing done here is passing the ENVVARS to the binary
# This script is still no distributed in the installer

import os
import sys
import logging
import ntpath
import datetime
from glob import glob
import fnmatch


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


ret=True # global res: all models
INSTALLER=False

def exe( cmd ):
    #print(cmd)
    silence=' > /dev/null 2>&1'
    #silence=''

    global INSTALLER
    if INSTALLER:
      cmd = 'env -i LD_LIBRARY_PATH={} DIAPATH={} DianaExeName={} {}'.format(
        'Qt:UserDLLs:tbb:diana/lib:diana/hasp:diana/intel:gcc/lib64:glib:png:zlib',
        'diana',
        'gm42',
        cmd)
      print('cmd: INSTALLER')

    print(cmd)

    res=os.system(cmd + silence) # returns the exit status
    if res: # not Ok
      ret=False
      print('FAILED')
    else:
      print('OK')


def main( argv ):
  try:

    global INSTALLER
    if len(argv)==1 and argv[0]=='INSTALLER':
      INSTALLER=True

    print('INSTALLER:{}'.format('Yes' if INSTALLER else 'No'))

    CONF=os.getenv('CONF')
    OUTDIR='test_output'
    if INSTALLER:
      BIN='geomec_shell'
      MODELS_PATH='Models/CORA' # TODO: by now this folder doesn't exist
    else:
      BIN='../../bin/{}/geomec_shell'.format(CONF)
      MODELS_PATH='../../Tests/Models/CORA'

    #so_files = glob('{}'.format(MODELS_PATH))
    #for file in so_files:
    #  print('file:{}'.format(file))


    # verified
    models = []
    for root, dirnames, filenames in os.walk(MODELS_PATH):
        for filename in fnmatch.filter(filenames, '*.gm4'):
            models.append(os.path.join(root, filename))
    #print('models:{}'.format(models))

    for model in models:
      filename=ntpath.basename(model)
      #filename=os.path.basename(model)
      dir=os.path.dirname(model)
      name,ext=os.path.splitext(filename)
      #print('-- dir:{} name:{}'.format(dir,name))
      print('')
      print('**** {}'.format(model))
      #print('')


      # ????????
      #../../bin/$CONF/geomec_shell test_output $CONF

      # 1)
      # ../../bin/release/geomec_shell 
      # ../../Tests/Models/CORA/quadlinear.gm4 
      # getmodelinfo 
      # test_output/quadlinear.modelinfo 
      # test_output/quadlinear.modelinfo
      cmd='{0} {1} getmodelinfo {2}/{3}.modelinfo {2}/{3}.modelinfo_result_summary'.format(BIN,model,OUTDIR,name)
      exe(cmd)
      ##############################################################################################
      # 2)
      file='{}/{}.reference'.format(dir,name)
      if os.path.isfile(file):

        # 2a)
        cmd='diff {0}/{1}.reference {2}/{1}.modelinfo > /dev/null'.format(dir,name,OUTDIR) # ????? /dev/null
        exe(cmd)

        # 2b)
        for version in ['1.7','2.0']:
          # 2b)
          # ../../bin/release/geomec_shell 
          # ../../Tests/Models/CORA/quadlinear.gm4 
          # getmodelinfo 
          # 1.7 
          # test_output/quadlinear.modelinfo_1.7 
          # test_output/quadlinear.modelinfo_1.7
          cmd='{0} {1} getmodelinfo {2} {3}/{4}.modelinfo_{2} {3}/{4}.modelinfo_result_summary_{2}'.format(BIN,model,version,OUTDIR,name)
          exe(cmd)

          cmd='diff {0}/{1}.reference {2}/{1}.modelinfo_{3} > /dev/null'.format(dir,name,OUTDIR,version)
          exe(cmd)
      ##############################################################################################
      # 3) runmodel
      # either test running model or recognition of wellpaths
      file='{}/{}.parameterfile'.format(dir,name)
      if os.path.isfile(file):
        # ../../bin/release/geomec_shell 
        # ../../Tests/Models/CORA/quadlinear.gm4 
        # runmodel 
        # ../../Tests/Models/CORA/quadlinear.parameterfile
        # ../../Tests/Models/CORA/quadlinear.selectedlsfs
        # test_output/quadlinear.modelinfo_response_parameters
        # test_output/quadlinear.modelinfo_result_summary
        # minimum_output

        if name == 'StagedTest':
          print("'StagedTest' skipped as it hangs")
          continue

        version='1.7'
        cmd='{0} {1} runmodel {2} {3}/{4}.parameterfile {3}/{4}.selectedlsfs {5}/{4}.response_parameters {5}/{4}.runmodel_result_summary minimum_output'.format(BIN,model,version,dir,name,OUTDIR)
        exe(cmd)

      # this is not Ok as include also 'Hexahedrom.gm4' and it shouldn't
      #else:

      ##############################################################################################
      # 4)
      if 'WellPath' in dir:

        #print('wellpath test: TO BE IMPLEMENTED')
        #print('wellpath test')
        # TODO: convert next to python
        # awk in the files name not in the content

        #cmd="echo {} | awk -F'[^0-9]+' '{ print $2 + $3; }'".format(model)
        cmd="echo " + model + " | awk -F'[^0-9]+' '{ print $2 + $3; }'"
        lines=os.popen(cmd).read()

        # verify this: 
        # $test_output/$base.modelinfo ... 
        # 'T.WP.1.NWP.1.gm4.modelinfo' or 'T.WP.1.NWP.1.modelinfo' ????
        cmd="grep -c '^well:New Wellpath' {}/{}.modelinfo".format(OUTDIR,name) 
        #print(cmd)
        lines_=os.popen(cmd).read()

        #print('lines:{} lines_:{}'.format(lines,lines_))

        if lines != lines_:
          ret=False
          print('FAILED')
        else:
          print('OK')


        #wp_total=`echo $base | awk -F'[^0-9]+' '{ print $2 + $3; }'`
        #if [ `grep -c '^well:New Wellpath' $test_output/$base.modelinfo` -ne $wp_total ]; then
        #  retval=$[retval + 1];
        #  echo TEST FAILED
        #fi



      #print(CMD)

      '''
      /bin/bash $TESTHOME/run_geomec_shell \
      $binpath \
      $path/$base.gm4 \
      getmodelinfo \
      $test_output/$base.modelinfo \
      $test_output/$base.modelinfo_result_summary
      '''



  except Exception as exception:
    show_error_details()


if __name__=='__main__':
  try:
    main( sys.argv[1:] )
  except Exception as exception:
    show_error_details()