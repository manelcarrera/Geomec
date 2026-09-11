# .o
unix:QMAKE_DISTCLEAN += ./debug/*.o
unix:QMAKE_DISTCLEAN += ./release/*.o
# TODO: .a
unix:QMAKE_DISTCLEAN += ./release/*.a
unix:QMAKE_DISTCLEAN += ./debug/*.a
# moc
unix:QMAKE_DISTCLEAN += ./Geology/h/moc_*.cpp