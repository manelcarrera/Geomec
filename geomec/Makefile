include $(DIAINCLUDE)/Mkrules.h

All:	Release_Geomec3.exe

Release_Geomec3.exe:
	ssexec -v -p $$/Lib -f ../Lib $(DSPMAKE) $(DSPMAKE_FLAGS)

Documentation:
	-( cd dox; $(MAKE) $(MFLAGS) )
