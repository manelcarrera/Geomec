
TEMPLATE = app

include(../qmake/config.pri)

CONFIG += console

include(../qmake/defines.pri)

TARGET   = dummy

include(../qmake/destination.pri)
include(../qmake/flags.pri)

BASE = ..

include(../qmake/dependencies.pri)

SOURCES  = dummy.cpp

QMAKE_EXTRA_TARGETS += diana
PRE_TARGETDEPS      += diana.pri

diana.target         = diana.pri
diana.commands       = $(SHELL) ./create.diana.pri
diana.depends        = ../dialibsenv.bat create.diana.pri

# Execute a script to check the environment.
# See http://colby.id.au/pre-pre-build-commands-with-qmake.

unix:check.environment.commands  = $(SHELL) ./check.environment.sh
win32:check.environment.commands = check.environment.bat
QMAKE_EXTRA_TARGETS += check.environment

# Hook our check.environment target in between qmake's Makefile update and
# the actual project target.

check.environment.hook.depends = check.environment

CONFIG(debug, debug|release) {
	check.environment.hook.target = Makefile.Debug
}

CONFIG(release, debug|release) {
	check.environment.hook.target = Makefile.Release
}

QMAKE_EXTRA_TARGETS += check.environment.hook

# Clean

# Removed the 'diana.pri' target from 'QMAKE_DISTCLEAN' to prevent the
# warnings 'Unable to find file for inclusion ../../qmake/diana.pri'.
# The dependencies are not properly processed when building the 'distclean'
# target (they should be processed in reverse order).

QMAKE_DISTCLEAN =

# Unit test

test.depends  =
test.commands =

test-debug.depends  =
test-debug.commands =

QMAKE_EXTRA_TARGETS += test test-debug

installer.depends =
installer.commands =

QMAKE_EXTRA_TARGETS += installer

