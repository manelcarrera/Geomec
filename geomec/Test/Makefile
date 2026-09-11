include $(DIAINCLUDE)/Mkrules.h

Prepare:
	-ssexec $(MAKE) $(MFLAGS) All

All:	Save_Load_dir GM3_Import_dir ShearWater_dir Meshregion_dir WellPath_dir Breakdown_dir

GM3_Import_dir:
	-( cd GM3_Import; $(MAKE) $(MFLAGS) )

ShearWater_dir:
	-( cd ShearWater; $(MAKE) $(MFLAGS) )

Meshregion_dir:
	-( cd Meshregion; $(MAKE) $(MFLAGS) )

Save_Load_dir:
	-( cd Save_Load; $(MAKE) $(MFLAGS) )

WellPath_dir:
	-( cd WellPath; $(MAKE) $(MFLAGS) )

Breakdown_dir:
	-( cd Breakdown; $(MAKE) $(MFLAGS) )