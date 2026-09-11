TEMPLATE = lib
#CONFIG += staticlib

BLOSCDIR = ../c-blosc-1.9.0

#DEFINES += SHUFFLE_SSE2_ENABLED SHUFFLE_AVX2_ENABLED
#DEFINES += __AVX2__

# Trying to compile the excluded files even sseming to be not necessary

#QMAKE_CXXFLAGS += -march=native 	# doesn't work
#QMAKE_CXXFLAGS += -mavx2 			# doesn't work either

#QMAKE_CFLAGS += -O3 -march=native -mfpmath=sse		# gives a different error
#QMAKE_CFLAGS += -mavx2 			# gives a different error


DONT_ADD_PATHS=1 # MCR: Better do it here by hand instead of customizing general case to fit 'blosc.pro'


include(../../qmake/all.pri)

DEFINES += HAVE_ZLIB HAVE_LZ4

# 'DESTDIR' needs to be set after 'all.pri', otherwise var is overwriten
CONFIG(debug, debug|release): 	DESTDIR = $${DLLROOT}/$$DESTINATION
CONFIG(release, debug|release): DESTDIR = $${DLLROOT}


# NEW

H=\ 
build/blosc \
blosc \
internal-complibs/lz4-1.7.2 \
internal-complibs/snappy-1.1.1 \
internal-complibs/zlib-1.2.8
for( folder, H ): HEADERS += $$BLOSCDIR/$$folder/*.h
# internal-complibs/zlib-1.2.8 : not used, but not problem if included

S_C=\
blosc \
internal-complibs/lz4-1.7.2 \
internal-complibs/zlib-1.2.8
for( folder, S_C ): SOURCES += $$BLOSCDIR/$$folder/*.c

S_CC=\
internal-complibs/snappy-1.1.1
for( folder, S_CC ): SOURCES += $$BLOSCDIR/$$folder/*.cc

S_EXCLUDE=\
blosc/shuffle-avx2.c \
blosc/bitshuffle-avx2.c \
internal-complibs/lz4-1.7.2/lz4hc.c
for( item, SOURCES): SOURCES_TMP += $$files( $$item )
SOURCES=$$SOURCES_TMP
for( file, S_EXCLUDE ): SOURCES -= $$BLOSCDIR/$$file


defined( SHOW_MESSAGE ,var ) {
	message( $${FILENAME}.pro : INCLUDEPATH:$$INCLUDEPATH )
	message( $${FILENAME}.pro : HEADERS:$$HEADERS )
	message( $${FILENAME}.pro : SOURCES:$$SOURCES )
}