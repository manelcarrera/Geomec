
unix {
	DEFINES += LINUX_X86_64
	DEFINES += GM_BUILD_TYPE_$(BUILD_TYPE)

	DEFINES += "FALSE=0"
	DEFINES += "TRUE=1"

	DEFINES += SKUA_NEW
}

win32:DEFINES -= UNICODE

CONFIG(debug, debug|release) {
	DEFINES += DEBUG
	DEFINES += _DEBUG
}

CONFIG(release, debug|release) {
	DEFINES += NDEBUG
}


unix {
	EXT  = .a
	EXT_S  = .so
}
win32 {
	EXT = .lib
	EXT_S = .dll
}

DIAINCLUDE = $$DIAPATH/src/include
