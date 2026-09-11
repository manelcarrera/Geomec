if [ "$DIAPATH" = "" ]; then
	echo "DIAPATH not defined"
	#exit 1
fi
if [ "$DIASRC" = "" ]; then
	echo "DIASRC not defined"
	#exit 1
fi
if [ "$INTEL" = "" ]; then
	echo "INTEL not defined"
	#exit 1
fi
if [ "$QTDIR" = "" ]; then
	echo "QTDIR not defined"
	#exit 1
fi
if [ `echo $LD_LIBRARY_PATH | grep -c $DIAPATH` = 0 ]; then
	echo "LD_LIBRARY_PATH does not contain \"$DIAPATH\""
	#exit 1
fi
if [ `echo $LD_LIBRARY_PATH | grep -c $INTEL` = 0 ]; then
	echo "LD_LIBRARY_PATH does not contain \"$INTEL\""
	#exit 1
fi
exit 0

