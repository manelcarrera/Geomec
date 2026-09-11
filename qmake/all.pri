include(config.pri) # debug_and_release, ordered
include(defines.pri)
include(globals.pri)		# MY_PATH | FILENAME | ROOT
include(basepath.pri)

equals( TYPE, 'Tests' ): FILENAME = TESTS$$basename( MY_PATH )
else: 	equals( TYPE, 'fw51' ): FILENAME = FW_$$basename( MY_PATH )
		else: FILENAME = $$basename( MY_PATH )
INCLUDEPATH_ = includepath/$$upper( $$FILENAME ).pri

exists( $$INCLUDEPATH_ ): include( $$INCLUDEPATH_ )

MSG=NOT found
exists( $$INCLUDEPATH_ ): MSG=found
message( all.pri : INCLUDEPATH_:$$INCLUDEPATH_ : $$MSG )	


# make paths shorter
# take care: '.' means any character
I_=
for( item, INCLUDEPATH ) {
	item_=$$replace( item, \.\.\/$$FILENAME, . )
	I_ += $$item_
}
INCLUDEPATH=$$I_

include(destination.pri)
include(flags.pri)
include(sources.pri)
include(clean.pri)

test.depends  =
test.commands =
QMAKE_EXTRA_TARGETS += test

message( ----------------------------- )