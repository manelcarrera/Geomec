include $(DIAINCLUDE)/Mkrules.h

All:	Release_Database_GM3.lib

Release_Database_GM3.lib:
	$(DSPMAKE) $(DSPMAKE_FLAGS)
	cp Release/DatabaseGM3.lib $(DSPLIB)

Documentation:
	-( cd dox; $(MAKE) $(MFLAGS) )
