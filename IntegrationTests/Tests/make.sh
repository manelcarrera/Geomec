#!/bin/sh

ROOT=$(cd ../.. && pwd)
LIB=$ROOT/lib


##########################################################
#
# 1) remove
#
##########################################################


cd $ROOT/bin/release

declare -a a=(
#
# libs
#
	libDianaStartup.a 
	libQUtil.a 
	libModelOperations.a
#
# bins
#
	Tests 
	geomec_shell 
	diana_app_new
)

for i in "${a[@]}"; do
   rm $i
done

##########################################################
#
# 2) make
#
##########################################################

declare -a b=(
#
# libs
#
	$LIB/QUtil
	$LIB/DianaStartup
	#
	$ROOT/Tests/ModelOperations
#
# bins
#
	$ROOT/Tests/Tests
	#
	$ROOT/geomec_shell
	$ROOT/diana_app_new
)

for i in "${b[@]}"; do
   cd $i && make
done


cd $ROOT/IntegrationTests/Tests