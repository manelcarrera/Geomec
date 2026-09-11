unix {
	QMAKE_CXXFLAGS += -std=c++14
}

win32 {
	QMAKE_CXXFLAGS += /Fd$(IntDir)
	QMAKE_CXXFLAGS += /MP8
	QMAKE_CXXFLAGS += /wd4100 /wd4189
}