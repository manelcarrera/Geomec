OBJECTS_DIR = $${BUILD}/.obj/$${CONF}/$${FILENAME}

MOC_DIR = $$OBJECTS_DIR

DESTDIR_ = $${BUILD}/bin/$${CONF}
DESTDIR = $$DESTDIR_/.a
TESTSDIR = $${ROOT}/test

#message( destination.pri : DESTDIR:$$DESTDIR )